#ifndef ALGAE_H
#define ALGAE_H

#include "Entity.h"
#include "EntityType.h"
#include <memory>
#include <vector>
#include <utility>
#include <random>

class Algae : public Entity {
    int age = 0;
    static constexpr int MAX_AGE = 20;
    static constexpr int REPRODUCE_AGE = 5;
public:
    EntityType getType() const override;
    std::unique_ptr<Entity> clone() const override;
    void tick(int x, int y, IOcean& current, IWritableOcean& next) override;
};

#endif 