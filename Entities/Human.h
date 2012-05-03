#ifndef OUTBREAK_ENTITIES_HUMAN_H
#define OUTBREAK_ENTITIES_HUMAN_H

#include "Entities/Entity.h"

namespace Outbreak
{

namespace Entities
{

class Human : public Entity
{
public:
    Human(Map::Location loc);
    virtual ~Human();

    virtual Map::Move::Enum Move(const Map::Map& map);
    virtual QColor Color() { return QColor(0, 255, 255); }

    ENTITY_DECLARATIONS(Human, Entity);

protected:
    virtual void CommitImpl();

private:
    class State;

    State *m_c;
    State *m_n;
};

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_ENTITIES_HUMAN_H
