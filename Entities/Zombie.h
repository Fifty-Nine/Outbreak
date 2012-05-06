#ifndef OUTBREAK_ENTITIES_ZOMBIE_H
#define OUTBREAK_ENTITIES_ZOMBIE_H

#include "Entities/Entity.h"

namespace Outbreak
{

namespace Entities
{

class Human;

class Zombie : public Entity
{
public:
    Zombie(Map::Location loc);
    virtual ~Zombie();

    virtual Map::Move::Enum Move(const Map::Map& map);
    virtual QColor Color() { return QColor(255, 0, 255); }

    bool MaybeEatBrains(const Human& victim);

    static int ZombieCount();

    ENTITY_DECLARATIONS(Zombie, Entity);

protected:
    virtual void CommitImpl();

private:
    Map::Move::Enum Ambling();
    Map::Move::Enum Idle(); 
    Map::Move::Enum Pursuit(const Map::Map& map);

    class State;

    State *m_c;
    State *m_n;
};

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_ENTITIES_ZOMBIE_H
