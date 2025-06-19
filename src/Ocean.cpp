#include "Ocean.h"
#include "Sand.h"          
#include "Algae.h"         
#include "HerbivoreFish.h" 
#include "PredatorFish.h"  
#include "Entity.h" 

#include <iostream>
#include <stdexcept> 

Ocean::Impl::Impl(int width, int height) : width(width), height(height) {
    grid.resize(width);
    for (int i = 0; i < width; ++i) {
        grid[i].resize(height);
        for (int j = 0; j < height; ++j) {
            grid[i][j] = EntityType::Sand;
        }
    }
}

Ocean::Impl::Impl(const Impl& other) : width(other.width), height(other.height), grid(other.grid) {}

EntityType Ocean::Impl::getCellType(int x, int y) const {
    if (!inBounds(x, y)) {
        throw std::out_of_range("Ocean::Impl::getCellType: Coordinates out of bounds");
    }
    return grid[x][y];
}

void Ocean::Impl::setCell(int x, int y, EntityType type) {
    if (!inBounds(x, y)) {
        throw std::out_of_range("Ocean::Impl::setCell: Coordinates out of bounds");
    }
    grid[x][y] = type;
}

bool Ocean::Impl::inBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

int Ocean::Impl::getWidth() const { return width; }
int Ocean::Impl::getHeight() const { return height; }


Ocean::Ocean(int width, int height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Ocean: Width and height must be positive.");
    }
    pimpl = std::make_unique<Impl>(width, height);
}

Ocean::~Ocean() = default;

Ocean::Ocean(const Ocean& other)
    : pimpl(std::make_unique<Impl>(*other.pimpl)) {
}

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
            nextOcean.setCell(x, y, getCellType(x, y));
        }
    }

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
                case EntityType::Sand:
                    continue;
                default:
                    throw std::runtime_error("Ocean::tick: Unknown entity type encountered.");
            }

            if (entity) {
                entity->tick(x, y, *this, nextOcean);
            }
        }
    }
    *this = std::move(nextOcean);
}

void Ocean::randomFill(int algaeCount, int herbivoreCount, int predatorCount) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, getWidth() - 1);
    std::uniform_int_distribution<int> distY(0, getHeight() - 1);

    auto placeEntities = [&](int count, EntityType type) {
        for (int i = 0; i < count;) {
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

int Ocean::countEntities(EntityType type) const {
    int count = 0;
    for (int x = 0; x < getWidth(); ++x) {
        for (int y = 0; y < getHeight(); ++y) {
            if (getCellType(x, y) == type) { 
                count++;
            }
        }
    }
    return count;
}
