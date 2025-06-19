#include "HerbivoreFish.h"

EntityType HerbivoreFish::getType() const { return EntityType::HerbivoreFish; }
std::unique_ptr<Entity> HerbivoreFish::clone() const { return std::make_unique<HerbivoreFish>(); }

void HerbivoreFish::tick(int x, int y, IOcean& current, IWritableOcean& next) {
    if (next.getCellType(x, y) != EntityType::Sand && next.getCellType(x,y) != EntityType::HerbivoreFish) {
        return;
    }

    age++;
    hunger++;
    if (age > MAX_AGE || hunger > MAX_HUNGER) {
        next.setCell(x, y, EntityType::Sand);
        return;
    }

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
        std::uniform_int_distribution<> dis(0, algaePositions.size() - 1);
        auto [ax, ay] = algaePositions[dis(gen)];
        new_x = ax;
        new_y = ay;
        hunger = std::max(0, hunger - HUNGER_DECREASE);
        next.setCell(ax, ay, EntityType::Sand);
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
            next.setCell(cx, cy, EntityType::HerbivoreFish);
        }
    }
    next.setCell(new_x, new_y, EntityType::HerbivoreFish);
    if (new_x != x || new_y != y) {
        next.setCell(x, y, EntityType::Sand);
    }
}