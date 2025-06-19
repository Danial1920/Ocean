#include "PredatorFish.h"

EntityType PredatorFish::getType() const { return EntityType::PredatorFish; }
std::unique_ptr<Entity> PredatorFish::clone() const { return std::make_unique<PredatorFish>(); }

void PredatorFish::tick(int x, int y, IOcean& current, IWritableOcean& next) {
    if (next.getCellType(x, y) != EntityType::Sand && next.getCellType(x,y) != EntityType::PredatorFish) {
        return;
    }

    age++;
    hunger++;
    if (age > MAX_AGE || hunger > MAX_HUNGER) {
        next.setCell(x, y, EntityType::Sand);
        return;
    }

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
        std::uniform_int_distribution<> dis(0, fishPositions.size() - 1);
        auto [fx, fy] = fishPositions[dis(gen)];
        new_x = fx;
        new_y = fy;
        hunger = std::max(0, hunger - HUNGER_DECREASE);
        next.setCell(fx, fy, EntityType::Sand);
    } else {
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
            std::uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
            std::tie(new_x, new_y) = possibleMoves[dis(gen)];
        }
    }

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
            std::uniform_int_distribution<> dis(0, emptyNeighbors.size() - 1);
            auto [cx, cy] = emptyNeighbors[dis(gen)];
            next.setCell(cx, cy, EntityType::PredatorFish);
        }
    }
    next.setCell(new_x, new_y, EntityType::PredatorFish);
    if (new_x != x || new_y != y) {
        next.setCell(x, y, EntityType::Sand);
    }
}