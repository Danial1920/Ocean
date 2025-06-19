#include "Algae.h"

EntityType Algae::getType() const { return EntityType::Algae; }
std::unique_ptr<Entity> Algae::clone() const { return std::make_unique<Algae>(); }

void Algae::tick(int x, int y, IOcean& current, IWritableOcean& next) {
    if (next.getCellType(x, y) != EntityType::Sand && next.getCellType(x, y) != EntityType::Algae) {
         return;
    }

    age++;
    if (age > MAX_AGE) {
        next.setCell(x, y, EntityType::Sand);
        return;
    }

    if (age >= REPRODUCE_AGE) {
        std::vector<std::pair<int, int>> emptyNeighbors;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (current.inBounds(nx, ny)) {
                    if (current.getCellType(nx, ny) == EntityType::Sand &&
                        next.getCellType(nx, ny) == EntityType::Sand) {
                        emptyNeighbors.emplace_back(nx, ny);
                    }
                }
            }
        }

        if (!emptyNeighbors.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, emptyNeighbors.size() - 1);
            auto [nx, ny] = emptyNeighbors[dis(gen)];
            next.setCell(nx, ny, EntityType::Algae);
        }
    }
    next.setCell(x, y, EntityType::Algae);
}