#ifndef ENTITY_H
#define ENTITY_H

#include "EntityType.h"
#include "IOcean.h"
#include "IWritableOcean.h"
#include <memory>

class Entity {
public:
    virtual ~Entity() = default;
    virtual EntityType getType() const = 0;
    virtual std::unique_ptr<Entity> clone() const = 0;
    virtual void tick(int x, int y, IOcean& current, IWritableOcean& next) = 0;
};

#endif 