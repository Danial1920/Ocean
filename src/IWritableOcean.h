#ifndef IWRITABLE_OCEAN_H
#define IWRITABLE_OCEAN_H

#include "IOcean.h"
#include "EntityType.h"

class IWritableOcean : public IOcean {
public:
    virtual void setCell(int x, int y, EntityType type) = 0;
};

#endif // IWRITABLE_OCEAN_H