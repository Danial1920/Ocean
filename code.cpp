#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <stdexcept>
#include <map>
#include <utility>
#include <tuple>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <fcntl.h>

// Функция для неблокирующего ввода
bool keyPressed() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}

// Очистка экрана
void clearScreen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape codes for clearing screen
}

enum class EntityType { Sand, Algae, HerbivoreFish, PredatorFish };

// Интерфейс для доступа к океану (только чтение)
class IOcean {
public:
    virtual ~IOcean() = default;
    virtual EntityType getCellType(int x, int y) const = 0;
    virtual bool inBounds(int x, int y) const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

// Интерфейс для записи в океан
class IWritableOcean : public IOcean {
public:
    virtual void setCell(int x, int y, EntityType type) = 0;
};

// Базовый класс для всех сущностей
class Entity {
public:
    virtual ~Entity() = default;
    virtual EntityType getType() const = 0;
    virtual std::unique_ptr<Entity> clone() const = 0;
    virtual void tick(int x, int y, IOcean& current, IWritableOcean& next) = 0;
};

// Пустая ячейка
class Sand : public Entity {
public:
    EntityType getType() const override { return EntityType::Sand; }
    std::unique_ptr<Entity> clone() const override { return std::make_unique<Sand>(); }
    void tick(int, int, IOcean&, IWritableOcean&) override {}
};

// Водоросли
class Algae : public Entity {
    int age = 0;
    static constexpr int MAX_AGE = 20;
    static constexpr int REPRODUCE_AGE = 5;
public:
    EntityType getType() const override { return EntityType::Algae; }
    std::unique_ptr<Entity> clone() const override { return std::make_unique<Algae>(); }

    void tick(int x, int y, IOcean& current, IWritableOcean& next) override {
        if (next.getCellType(x, y) != EntityType::Sand) return;

        age++;
        if (age > MAX_AGE) return;

        // Размножение в соседние клетки
        if (age >= REPRODUCE_AGE) {
            std::vector<std::pair<int, int>> emptyNeighbors;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (current.inBounds(nx, ny)) {
                        if (next.getCellType(nx, ny) == EntityType::Sand && 
                            current.getCellType(nx, ny) == EntityType::Sand) {
                            emptyNeighbors.emplace_back(nx, ny);
                        }
                    }
                }
            }

            if (!emptyNeighbors.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, emptyNeighbors.size()-1);
                auto [nx, ny] = emptyNeighbors[dis(gen)];
                next.setCell(nx, ny, EntityType::Algae);
            }
        }

        next.setCell(x, y, EntityType::Algae);
    }
};

// Травоядная рыба
class HerbivoreFish : public Entity {
    int age = 0;
    int hunger = 0;
    static constexpr int MAX_AGE = 50;
    static constexpr int MAX_HUNGER = 10;
    static constexpr int REPRODUCE_AGE = 10;
    static constexpr int HUNGER_DECREASE = 5;
public:
    EntityType getType() const override { return EntityType::HerbivoreFish; }
    std::unique_ptr<Entity> clone() const override { return std::make_unique<HerbivoreFish>(); }

    void tick(int x, int y, IOcean& current, IWritableOcean& next) override {
        if (next.getCellType(x, y) != EntityType::Sand) return;

        age++;
        hunger++;
        if (age > MAX_AGE || hunger > MAX_HUNGER) return;

        // Поиск пищи (водорослей)
        std::vector<std::pair<int, int>> algaePositions;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (current.inBounds(nx, ny)) {
                    if (current.getCellType(nx, ny) == EntityType::Algae) {
                        algaePositions.emplace_back(nx, ny);
                    }
                }
            }
        }

        int new_x = x, new_y = y;
        if (!algaePositions.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, algaePositions.size()-1);
            auto [ax, ay] = algaePositions[dis(gen)];
            new_x = ax;
            new_y = ay;
            hunger = std::max(0, hunger - HUNGER_DECREASE);
        } else {
            // Случайное перемещение
            std::vector<std::pair<int, int>> possibleMoves;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (current.inBounds(nx, ny)) {
                        if (next.getCellType(nx, ny) == EntityType::Sand) {
                            possibleMoves.emplace_back(nx, ny);
                        }
                    }
                }
            }

            if (!possibleMoves.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, possibleMoves.size()-1);
                std::tie(new_x, new_y) = possibleMoves[dis(gen)];
            }
        }

        // Размножение
        if (age >= REPRODUCE_AGE) {
            std::vector<std::pair<int, int>> emptyNeighbors;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = new_x + dx, ny = new_y + dy;
                    if (current.inBounds(nx, ny)) {
                        if (next.getCellType(nx, ny) == EntityType::Sand) {
                            emptyNeighbors.emplace_back(nx, ny);
                        }
                    }
                }
            }

            if (!emptyNeighbors.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, emptyNeighbors.size()-1);
                auto [cx, cy] = emptyNeighbors[dis(gen)];
                next.setCell(cx, cy, EntityType::HerbivoreFish);
            }
        }

        next.setCell(new_x, new_y, EntityType::HerbivoreFish);
    }
};

// Хищная рыба
class PredatorFish : public Entity {
    int age = 0;
    int hunger = 0;
    static constexpr int MAX_AGE = 70;
    static constexpr int MAX_HUNGER = 15;
    static constexpr int REPRODUCE_AGE = 15;
    static constexpr int HUNGER_DECREASE = 7;
public:
    EntityType getType() const override { return EntityType::PredatorFish; }
    std::unique_ptr<Entity> clone() const override { return std::make_unique<PredatorFish>(); }

    void tick(int x, int y, IOcean& current, IWritableOcean& next) override {
        if (next.getCellType(x, y) != EntityType::Sand) return;

        age++;
        hunger++;
        if (age > MAX_AGE || hunger > MAX_HUNGER) return;

        // Поиск пищи (травоядных рыб)
        std::vector<std::pair<int, int>> fishPositions;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (current.inBounds(nx, ny)) {
                    if (current.getCellType(nx, ny) == EntityType::HerbivoreFish) {
                        fishPositions.emplace_back(nx, ny);
                    }
                }
            }
        }

        int new_x = x, new_y = y;
        if (!fishPositions.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, fishPositions.size()-1);
            auto [fx, fy] = fishPositions[dis(gen)];
            new_x = fx;
            new_y = fy;
            hunger = std::max(0, hunger - HUNGER_DECREASE);
        } else {
            // Случайное перемещение
            std::vector<std::pair<int, int>> possibleMoves;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (current.inBounds(nx, ny)) {
                        if (next.getCellType(nx, ny) == EntityType::Sand) {
                            possibleMoves.emplace_back(nx, ny);
                        }
                    }
                }
            }

            if (!possibleMoves.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, possibleMoves.size()-1);
                std::tie(new_x, new_y) = possibleMoves[dis(gen)];
            }
        }

        // Размножение
        if (age >= REPRODUCE_AGE) {
            std::vector<std::pair<int, int>> emptyNeighbors;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = new_x + dx, ny = new_y + dy;
                    if (current.inBounds(nx, ny)) {
                        if (next.getCellType(nx, ny) == EntityType::Sand) {
                            emptyNeighbors.emplace_back(nx, ny);
                        }
                    }
                }
            }

            if (!emptyNeighbors.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, emptyNeighbors.size()-1);
                auto [cx, cy] = emptyNeighbors[dis(gen)];
                next.setCell(cx, cy, EntityType::PredatorFish);
            }
        }

        next.setCell(new_x, new_y, EntityType::PredatorFish);
    }
};

// Реализация океана с использованием Pimpl
class Ocean : public IWritableOcean {
public:
    Ocean(int width, int height);
    ~Ocean() override;
    Ocean(const Ocean& other);
    Ocean(Ocean&& other) noexcept;
    Ocean& operator=(const Ocean& other);
    Ocean& operator=(Ocean&& other) noexcept;

    // IWritableOcean interface
    EntityType getCellType(int x, int y) const override;
    void setCell(int x, int y, EntityType type) override;
    bool inBounds(int x, int y) const override;
    int getWidth() const override;
    int getHeight() const override;

    void tick();
    void randomFill(int algaeCount, int herbivoreCount, int predatorCount);

    template <EntityType T>
    int countEntities() const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// Внутренняя реализация океана
class Ocean::Impl : public IWritableOcean {
public:
    Impl(int width, int height) 
        : width(width), height(height), 
          grid(width, std::vector<EntityType>(height, EntityType::Sand)) {}

    Impl(const Impl& other)
        : width(other.width), height(other.height), grid(other.grid) {}

    EntityType getCellType(int x, int y) const override {
        if (!inBounds(x, y)) throw std::out_of_range("Coordinates out of bounds");
        return grid[x][y];
    }

    void setCell(int x, int y, EntityType type) override {
        if (!inBounds(x, y)) throw std::out_of_range("Coordinates out of bounds");
        grid[x][y] = type;
    }

    bool inBounds(int x, int y) const override {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    std::vector<std::vector<EntityType>> grid;
    int width;
    int height;
};

// Реализация методов Ocean с использованием Pimpl
Ocean::Ocean(int width, int height) 
    : pimpl(std::make_unique<Impl>(width, height)) {}

Ocean::~Ocean() = default;

Ocean::Ocean(const Ocean& other) 
    : pimpl(std::make_unique<Impl>(*other.pimpl)) {}

Ocean::Ocean(Ocean&& other) noexcept = default;

Ocean& Ocean::operator=(const Ocean& other) {
    if (this != &other) {
        pimpl = std::make_unique<Impl>(*other.pimpl);
    }
    return *this;
}

Ocean& Ocean::operator=(Ocean&& other) noexcept = default;

EntityType Ocean::getCellType(int x, int y) const { 
    return pimpl->getCellType(x, y); 
}

void Ocean::setCell(int x, int y, EntityType type) { 
    pimpl->setCell(x, y, type); 
}

bool Ocean::inBounds(int x, int y) const { 
    return pimpl->inBounds(x, y); 
}

int Ocean::getWidth() const { 
    return pimpl->getWidth(); 
}

int Ocean::getHeight() const { 
    return pimpl->getHeight(); 
}

void Ocean::tick() {
    Ocean nextOcean(getWidth(), getHeight());
    
    for (int x = 0; x < getWidth(); ++x) {
        for (int y = 0; y < getHeight(); ++y) {
            EntityType type = getCellType(x, y);
            std::unique_ptr<Entity> entity;
            
            switch (type) {
                case EntityType::Algae:
                    entity = std::make_unique<Algae>();
                    break;
                case EntityType::HerbivoreFish:
                    entity = std::make_unique<HerbivoreFish>();
                    break;
                case EntityType::PredatorFish:
                    entity = std::make_unique<PredatorFish>();
                    break;
                default:
                    continue;
            }
            
            entity->tick(x, y, *this, nextOcean);
        }
    }
    
    *this = std::move(nextOcean);
}

void Ocean::randomFill(int algaeCount, int herbivoreCount, int predatorCount) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, getWidth()-1);
    std::uniform_int_distribution<int> distY(0, getHeight()-1);
    
    auto placeEntities = [&](int count, EntityType type) {
        for (int i = 0; i < count; ) {
            int x = distX(gen);
            int y = distY(gen);
            if (getCellType(x, y) == EntityType::Sand) {
                setCell(x, y, type);
                i++;
            }
        }
    };
    
    placeEntities(algaeCount, EntityType::Algae);
    placeEntities(herbivoreCount, EntityType::HerbivoreFish);
    placeEntities(predatorCount, EntityType::PredatorFish);
}

template <EntityType T>
int Ocean::countEntities() const {
    int count = 0;
    for (int x = 0; x < getWidth(); ++x) {
        for (int y = 0; y < getHeight(); ++y) {
            if (getCellType(x, y) == T) {
                count++;
            }
        }
    }
    return count;
}

int main() {
    try {
        Ocean ocean(50, 50);
        ocean.randomFill(100, 20, 5);
        
        for (int i = 0; i < 100; ++i) {
            ocean.tick();
            
            clearScreen(); // Очищаем экран перед каждым выводом
            
            std::cout << "Tick " << i + 1 << ":\n";
            std::cout << "  Algae: " << ocean.countEntities<EntityType::Algae>() << "\n";
            std::cout << "  HerbivoreFish: " << ocean.countEntities<EntityType::HerbivoreFish>() << "\n";
            std::cout << "  PredatorFish: " << ocean.countEntities<EntityType::PredatorFish>() << "\n";
            std::cout << "\nPress space to pause, any other key to continue...";
            
            // Добавляем задержку для визуализации
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            
            // Проверка нажатия клавиши
            if (keyPressed()) {
                char c = getchar();
                if (c == ' ') {
                    std::cout << "\nSimulation paused. Press any key to continue...";
                    getchar(); // Ждем нажатия для продолжения
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}