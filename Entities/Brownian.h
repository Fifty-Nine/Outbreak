#ifndef OUTBREAK_BROWNIAN_ENTITY_H
#define OUTBREAK_BROWNIAN_ENTITY_H

#include "Entities/Entity.h"
#include "Map/Location.h"
#include "Map/Move.h"

namespace Outbreak
{

namespace Entities
{

class Brownian : public Entity
{
public:
    Brownian(Map::Location location);

    virtual Map::Move::Enum Move(const Map::Map& map);
    virtual QColor Color() { return QColor("red"); }

protected:
    virtual void CommitImpl() { }

    ENTITY_DECLARATIONS(Brownian, Entity);
};

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_BROWNIAN_ENTITY_H
