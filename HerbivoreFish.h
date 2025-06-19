#ifndef HERBIVORE_FISH_H
#define HERBIVORE_FISH_H

#include "Entity.h"
#include "EntityType.h"
#include <memory>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <tuple>

class HerbivoreFish : public Entity {
    int age = 0;
    int hunger = 0;
    static constexpr int MAX_AGE = 50;
    static constexpr int MAX_HUNGER = 10;
    static constexpr int REPRODUCE_AGE = 10;
    static constexpr int HUNGER_DECREASE = 5;
public:
    EntityType getType() const override;
    std::unique_ptr<Entity> clone() const override;
    void tick(int x, int y, IOcean& current, IWritableOcean& next) override;
};

#endif 