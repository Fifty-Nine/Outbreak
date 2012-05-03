#ifndef OUTBREAK_ENTITIES_FLOCKER_H
#define OUTBREAK_ENTITIES_FLOCKER_H

#include "Entities/Entity.h"

namespace Outbreak
{

namespace Entities
{

class Flocker : public Entity
{
public:
    Flocker(Map::Location loc, bool quiet = true);
    virtual ~Flocker();

    Map::Location Destination() const;

    virtual Map::Move::Enum Move(const Map::Map& map);
    virtual QColor Color() 
    { return m_quiet ? QColor(0, 255, 0) : QColor(255,255,255); }

    ENTITY_DECLARATIONS(Flocker, Entity);

protected:
    virtual void CommitImpl();

private:
    Map::Move::Enum LeaderMove(const Map::Map& map);
    Map::Move::Enum FollowerMove(const Map::Map& map);
    int Distance(Map::Location loc) const;

    class State;

    State *m_c;
    State *m_n;

    bool m_quiet;
}; 

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_ENTITIES_FLOCKER_H
