#ifndef OCEAN_H
#define OCEAN_H

#include "IWritableOcean.h"
#include "EntityType.h"
#include <memory>
#include <vector>
#include <stdexcept>
#include <random>

class Ocean : public IWritableOcean {
public:
    Ocean(int width, int height);
    ~Ocean() override;
    Ocean(const Ocean& other);
    Ocean(Ocean&& other) noexcept;
    Ocean& operator=(const Ocean& other);
    Ocean& operator=(Ocean&& other) noexcept;

    EntityType getCellType(int x, int y) const override;
    bool inBounds(int x, int y) const override;
    int getWidth() const override;
    int getHeight() const override;

    void setCell(int x, int y, EntityType type) override;

    void tick();
    void randomFill(int algaeCount, int herbivoreCount, int predatorCount);

    template <EntityType T>
    int countEntities() const;

private:
    class Impl : public IWritableOcean {
    public:
        Impl(int width, int height);
        Impl(const Impl& other);

        EntityType getCellType(int x, int y) const override;
        void setCell(int x, int y, EntityType type) override;
        bool inBounds(int x, int y) const override;
        int getWidth() const override;
        int getHeight() const override;

        std::vector<std::vector<EntityType>> grid;
        int width;
        int height;
    };

    std::unique_ptr<Impl> pimpl;
};

#endif 