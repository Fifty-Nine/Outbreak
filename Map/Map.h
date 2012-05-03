#ifndef OUTBREAK_MAP_H
#define OUTBREAK_MAP_H

#include <QSet>
#include <QVector>

#include "Entities/Entity.h"
#include "Entities/EntityDispatch.h"
#include "Map/Location.h"
#include "Map/Move.h"

namespace Outbreak
{

namespace Entities { class Entity; }

namespace Map
{

class Map
{
    typedef QMultiHash<Location, Entities::Entity*> EntityMap;
    typedef QVector<Entities::Entity*> EntityList;
    typedef QHash<Location, EntityList> EntityTable;
public:
    Map();
    Map(int width, int height, const QVector<Entities::Entity*>& entities);
    Map(const Map& map);
    ~Map();

    int Width() const { return m_width; }
    int Height() const { return m_height; }

    Map Iterate(Entities::EntityDispatcherBase& dispatcher) const;

    const EntityMap& Entities() const { return m_entities; }

    template<class T>
    QList<T*> Entities(Location loc) const;

    template<class T>
    bool AnyEntities(Location loc, T* except_p = NULL) const;

    template< class R, class T >
    R* NearestNeighbor(T* entity_p, int radius) const;

    Location RandomLocation() const;

private:
    static void ResolveCell(QVector<Entities::Entity*>& entities, 
        Entities::EntityDispatcherBase& dispatcher);
    inline bool InBounds(Location loc) const;

    int m_width; int m_height;
    EntityMap m_entities;
};

template< class R, class T >
R* Map::NearestNeighbor(T* entity_p, int radius) const
{
    Location loc = entity_p->Location();

    for (int b = 1; b < radius; b++)
    {
        for (int i = -b; i <= b; i++)
        {
            Location p1(loc.X - i, loc.Y - b);
            
            for (EntityMap::const_iterator it = m_entities.find(p1);
                 it != m_entities.end() && it.key() == p1; ++it)
            {
                R* value_p = dynamic_cast<R*>( it.value() );
                if (value_p) return value_p;
            }

            Location p2(loc.X - i, loc.Y + b);

            for (EntityMap::const_iterator it = m_entities.find(p2);
                 it != m_entities.end() && it.key() == p2; ++it)
            {
                R* value_p = dynamic_cast<R*>( it.value() );
                if (value_p) return value_p;
            }

            Location p3(loc.X - b, loc.Y - i);

            for (EntityMap::const_iterator it = m_entities.find(p3);
                 it != m_entities.end() && it.key() == p3; ++it)
            {
                R* value_p = dynamic_cast<R*>( it.value() );
                if (value_p) return value_p;
            }

            Location p4(loc.X + b, loc.Y - i);

            for (EntityMap::const_iterator it = m_entities.find(p4);
                 it != m_entities.end() && it.key() == p4; ++it)
            {
                R* value_p = dynamic_cast<R*>( it.value() );
                if (value_p) return value_p;
            }
        }
    }

    return NULL;
}

template<class T>
QList<T*> Map::Entities(Location loc) const
{
    using namespace Entities;
    QList<T*> result;
    QList<Entity*> entities = m_entities.values(loc);

    for (QList<Entity*>::const_iterator it = entities.begin(); 
         it != entities.end(); ++it)
    {
        T* t_p = dynamic_cast<T*>(*it);

        if (t_p)
        {
            result.append(t_p);
        }
    }

    return result;
}

template<class T>
bool Map::AnyEntities(Location loc, T* except_p) const
{
    using namespace Entities;
    QList<Entity*> entities = m_entities.values(loc);

    for (QList<Entity*>::const_iterator it = entities.begin();
         it != entities.end(); ++it)
    {
        T* t_p = (*it != except_p) ? dynamic_cast<T*>(*it) : 0;

        if (t_p)
        {
            return true;
        }
    }

    return false;
}

} // namespace Map

} // namespace Outbreak

#endif // OUTBREAK_MAP_H
