#ifndef OUTBREAK_ENTITIES_FIREFLY_H
#define OUTBREAK_ENTITIES_FIREFLY_H

#include "Entities/Entity.h"
#include "Map/Location.h"
#include "Map/Move.h"

namespace Outbreak
{

namespace Entities
{

class Firefly : public Entity
{
public:
    Firefly(Map::Location location, bool debug = false);
    virtual ~Firefly();

    virtual Map::Move::Enum Move(const Map::Map& map);
    virtual QColor Color();

    ENTITY_DECLARATIONS(Firefly, Entity);

protected:
    virtual void CommitImpl();

private:
    void NextColor();
    void ExamineNeighbor(Firefly *neighbor_p);

    class State;

    State *m_c;
    State *m_n;

    bool m_debug;
};

} // namespace Entities

} //namespace Outbreak

#endif // OUTBREAK_ENTITIES_FIREFLY_H
