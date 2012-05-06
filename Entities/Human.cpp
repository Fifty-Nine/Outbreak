#include "Entities/Human.h"
#include "Entities/Zombie.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Entities
{

namespace
{

const int AVOIDANCE_FACTOR = 15;
const int ENDURANCE = 10;

}

class Human::State
{
public:
    Map::Location dest;
    int counter;
    enum Action
    {
        MOVING, 
        PANICKED,
        TIRED
    } action;

    State(Map::Location loc) : 
        dest(loc),
        counter(0), 
        action(MOVING)
    {
    }


};


Human::Human(Map::Location loc) :
    Entity(loc),
    m_c(new State(loc)),
    m_n(new State(loc))
{
}

Human::~Human()
{
    delete m_c;
    delete m_n;
}

Map::Move::Enum Human::Move(const Map::Map& map)
{
    *m_n = *m_c;

    if (m_n->dest == Location())
    {
        m_n->dest = map.RandomLocation();
    }

    if ((m_n->action == State::TIRED) && (m_n->counter > 0))
    {
        m_n->counter--;
        return Map::Move::None;
    }

    Zombie *nearest_p = map.NearestNeighbor<Zombie>(this, AVOIDANCE_FACTOR);

    if (nearest_p)
    {
        if (m_n->action == State::PANICKED && (--m_n->counter == 0))
        {
            m_n->action = State::TIRED;
            m_n->counter = 1;
        }
        else if (m_n->action != State::PANICKED)
        {
            m_n->action = State::PANICKED;
            m_n->counter = ENDURANCE;
        }

        return 
            Map::Move::Opposite(Location().DirectionTo(nearest_p->Location()));
    }
    else if (m_n->action == State::PANICKED)
    {
        if (--m_n->counter == 0)
        {
            Human *human_p = map.NearestNeighbor<Human>(this, AVOIDANCE_FACTOR);

            if (human_p && human_p->m_c->action == State::PANICKED)
            {
                m_n->counter = ENDURANCE;
                return Map::Move::Opposite(Location().DirectionTo(human_p->Location()));
            }
            
            m_n->action = State::MOVING;
            m_n->counter = 0;
            return Location().DirectionTo(m_n->dest);
        }

        return Map::Move::Random();
    }
    else if (m_n->action != State::MOVING)
    {
        m_n->action = State::MOVING; 
        m_n->counter = 0;
    }

    Human *human_p = map.NearestNeighbor<Human>(this, AVOIDANCE_FACTOR);

    if (human_p && human_p->m_c->action == State::PANICKED)
    {
        m_n->action = State::PANICKED;
        m_n->counter = ENDURANCE;
        return Map::Move::Random();
    }

    if (m_n->counter == 1)
    {
        m_n->counter = 0;
        return Map::Move::None;
    }

    m_n->counter = 1;

    return Location().DirectionTo(m_n->dest);
}

void Human::CommitImpl()
{
    std::swap(m_c, m_n);
}

} // namespace Entities

} // namespace Outbreak
