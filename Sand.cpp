#include "Sand.h"

EntityType Sand::getType() const { return EntityType::Sand; }
std::unique_ptr<Entity> Sand::clone() const { return std::make_unique<Sand>(); }
void Sand::tick(int, int, IOcean&, IWritableOcean&) {}