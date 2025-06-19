#ifndef IOCEAN_H
#define IOCEAN_H

#include "EntityType.h"

class IOcean {
public:
    virtual ~IOcean() = default;
    virtual EntityType getCellType(int x, int y) const = 0;
    virtual bool inBounds(int x, int y) const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

#endif 