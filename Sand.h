#ifndef SAND_H
#define SAND_H

#include "Entity.h"
#include "EntityType.h"
#include <memory>

class Sand : public Entity {
public:
    EntityType getType() const override;
    std::unique_ptr<Entity> clone() const override;
    void tick(int, int, IOcean&, IWritableOcean&) override;
};

#endif 