#ifndef OUTBREAK_ENTITY_H
#define OUTBREAK_ENTITY_H

#include <cassert>
#include <typeinfo>

#include <QColor>

#include "Map/Location.h"
#include "Util/Dispatcher.h"

#define ENTITY_DECLARATIONS(type, parent)\
    IMPLEMENT_DOUBLE_DISPATCH(type, parent)

namespace Outbreak
{

namespace Map { class Map; }

namespace Entities
{

class Entity;

class Entity
{
public:
    Entity(Map::Location location) : 
        m_c_location(location), 
        m_n_location(location),
        m_alive(true) { }
    virtual ~Entity() { }

    Map::Location Location() const { return m_c_location; }
    Map::Location NewLocation() const { return m_n_location; }
    void SetLocation(Map::Location value) { m_n_location = value; }

    virtual Map::Move::Enum Move(const Map::Map& map) = 0;
    void Commit() { m_c_location = m_n_location; CommitImpl(); }
    virtual QColor Color() = 0;

    bool Alive() { return m_alive; }
    void Kill() { m_alive = false; }

    IMPLEMENT_DOUBLE_DISPATCH_BASE(Entity);

protected:
    virtual void CommitImpl() = 0;

private:
    Map::Location m_c_location;
    Map::Location m_n_location;
    bool m_alive;
};

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_ENTITY_H
