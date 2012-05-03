#include <cmath>
#include <cstdlib>
#include <cstdint>

#include <QDebug>

#include "Entities/Flocker.h"
#include "Map/Map.h"

namespace
{

const int COHESION_FACTOR = 20;
const int AVOIDANCE_FACTOR = 8;

using namespace Outbreak;

#define FLOCKDEBUG(out) if (!m_quiet) (qDebug() << (uintptr_t)this << out)

}

namespace Outbreak
{

namespace Entities
{

class Flocker::State
{
public:
    Flocker *leader_p;
    Map::Location dest;

    State(Map::Location location) :
        leader_p(NULL), dest(location)
    { }
};

Flocker::Flocker(Map::Location location, bool quiet) :
    Entity(location), 
    m_c(new State(location)),
    m_n(new State(location)),
    m_quiet(quiet)
{
}

Flocker::~Flocker()
{
    delete m_c;
    delete m_n;
}

Map::Location Flocker::Destination() const
{
    if (m_c->leader_p)
    {
        return m_c->leader_p->Destination();
    }

    return m_c->dest;
}

Map::Move::Enum Flocker::Move(const Map::Map& map)
{ 
    *m_n = *m_c;
    return m_n->leader_p ? FollowerMove(map) : LeaderMove(map); 
}

void Flocker::CommitImpl()
{
    std::swap(m_c, m_n);
}

Map::Move::Enum Flocker::LeaderMove(const Map::Map& map)
{
    FLOCKDEBUG("Leader");
    Flocker* neighbor_p = map.NearestNeighbor<Flocker>(this, COHESION_FACTOR);

    if (neighbor_p)
    {
        FLOCKDEBUG("Found neighbor!" << (uintptr_t)neighbor_p);

        Flocker* new_leader_p = neighbor_p->m_c->leader_p
            ? neighbor_p->m_c->leader_p : neighbor_p;

        uintptr_t us = (uintptr_t)this;
        uintptr_t them = (uintptr_t)new_leader_p;

        if (them > us)
        {
            FLOCKDEBUG("Now following new leader" << new_leader_p);
            m_n->leader_p = new_leader_p;
            return Location().DirectionTo(new_leader_p->Location());
        }
        else
        {
            FLOCKDEBUG("Still leader.");
        }
    }

    if (m_n->dest == Location())
    {
        m_n->dest = map.RandomLocation();
        FLOCKDEBUG("Going to new location!" <<
                   "(" << m_n->dest.X << m_n->dest.Y << ")");
    }

    return Location().DirectionTo(m_n->dest);
}

Map::Move::Enum Flocker::FollowerMove(const Map::Map& map)
{
    FLOCKDEBUG("Follower");
    if (map.Entities().count(Location()) > 1)
    {
        FLOCKDEBUG("Collision");
        return Map::Move::Random();
    }

    Flocker* neighbor_p = map.NearestNeighbor<Flocker>(this, COHESION_FACTOR);
    
    if (!neighbor_p)
    {
        FLOCKDEBUG("No neighbors. Leading.");
        m_n->leader_p = NULL;
        m_n->dest = map.RandomLocation();
        return Location().DirectionTo(m_n->dest);
    }

    Map::Location neighbor_loc = neighbor_p->Location();

    if (Distance(neighbor_loc) <= AVOIDANCE_FACTOR)
    {
        FLOCKDEBUG("Avoiding neighbor"  << (uintptr_t)neighbor_p);
        return Map::Move::Opposite(Location().DirectionTo(neighbor_loc));
    }

    return Location().DirectionTo(m_n->leader_p->Destination());
}

int Flocker::Distance(Map::Location loc) const
{
    Map::Location us = Location();

    return (abs(loc.X - us.X) + abs(loc.Y - us.Y));
}

} // namespace Entities

}
