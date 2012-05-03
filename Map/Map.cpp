#include <boost/bind.hpp>

#include "Entities/Entity.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Map
{

using namespace Entities;

Map::Map() :
    m_width(0),
    m_height(0)
{
}

Map::Map(int width, int height, const QVector<Entity*>& entities) : 
    m_width(width), 
    m_height(height)
{
    for (QVector<Entity*>::const_iterator it = entities.begin(); 
        it != entities.end(); ++it)
    {
        m_entities.insert((*it)->Location(), *it);
    }
}

Map::Map(const Map& map) : 
    m_width(map.m_width), 
    m_height(map.m_height), 
    m_entities(map.m_entities)
{
}

Map::~Map()
{
}

bool Map::InBounds(Location loc) const
{
    return (loc.X < m_width) && (loc.X >= 0)
        && (loc.Y < m_height) && (loc.Y >= 0);
}

Location Map::RandomLocation() const
{
    return Location(rand() % m_width, rand() % m_height);
}

void Map::ResolveCell(EntityList& entities, 
    Entities::EntityDispatcherBase& dispatcher)
{
    assert(entities.count());

    Location loc = entities[0]->NewLocation();

    QList<Entity*> new_entities;

    for (EntityList::const_iterator it = entities.begin(); 
         it != entities.end(); ++it)
    {
        Entity* lhs = *it;
        
        for (EntityList::const_iterator jt = it + 1; jt != entities.end(); jt++)
        {
            if (!lhs->Alive())
            {
                break;
            }

            Entity* rhs = *jt;

            if (!rhs->Alive())
            {
                continue;
            }

            new_entities.append(dispatcher.Dispatch(*lhs, *rhs));
        }
    }

    for (EntityList::const_iterator it = entities.begin(); 
         it != entities.end(); ++it)
    {
        Entity *entity_p( *it );
        if (entity_p->Alive())
        {
            entity_p->SetLocation(loc);
            entity_p->Commit();
        }
    }

    for (QList<Entity*>::const_iterator it = new_entities.begin(); 
         it != new_entities.end(); ++it)
    {
        Entity *entity_p = *it;
        entity_p->SetLocation(loc);
        entity_p->Commit();
        entities.append(entity_p);
    }
}

Map Map::Iterate(EntityDispatcherBase& dispatcher) const
{
    EntityTable new_cells;

    QList<Entity*> entities(m_entities.values());

    #pragma omp parallel for
    for (int i = 0; i < entities.count(); i++)
    {
        Entity *entity_p = entities[i];
        Move::Enum move = entity_p->Move(*this);
        Location loc = entity_p->Location();

        Location new_location = loc.Neighbor(move);

        if (!InBounds(new_location))
        {
            new_location = loc;
        }

        entity_p->SetLocation(new_location);
    }

    for (int i = 0; i < entities.count(); i++)
    {
        Entity *entity_p = entities[i];
        new_cells[entity_p->NewLocation()].append(entity_p);
    }

    /*
    for (EntityMap::const_iterator it = m_entities.begin(); 
        it != m_entities.end(); ++it )
    {
        Entity* entity_p = it.value();
        Move::Enum move = entity_p->Move(*this);

        Location new_location = it.key().Neighbor(move);

        if (!InBounds(new_location))
        {
            new_location = it.key();
        }

        new_cells[new_location].append(entity_p);
    }
*/

    Map new_map;
    new_map.m_width = m_width;
    new_map.m_height = m_height;

    QList<EntityList> new_entities(new_cells.values());

#pragma omp parallel for
    for (int i = 0; i < new_entities.count(); i++)
    {
        ResolveCell(new_entities[i], dispatcher);
    }

    for (int i = 0; i < new_entities.count(); i++)
    {
        const EntityList& list(new_entities[i]);
        for (int j = 0; j < list.count(); j++)
        {
            Entity *entity_p(list[j]);
            if (entity_p->Alive())
            {
                new_map.m_entities.insert(entity_p->Location(), entity_p);
            }
            else
            {
                delete entity_p;
            }
        }
    }

/*    for (EntityTable::const_iterator it = new_cells.begin(); 
         it < new_cells.end(); ++it)
    {
        ResolveCell(dispatcher, new_map, it.key(), it.value());
    }*/

    return new_map;
}

} // namespace Map

} // namespace Outbreak
