#include <QVector2D>

#include "Entities/Zombie.h"
#include "Entities/Human.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Entities
{

namespace
{

const int DETECT_RANGE = 30;
const int GIVE_UP_RANGE = 100;

int zombie_count = 0;

}


class Zombie::State
{
public:
    enum 
    {
        AMBLING,
        IDLE,
        PURSUIT
    } state;
    int counter;
    Map::Move::Enum amble_dir;
    Human* prey;
    bool wait;

    State(int counter, Map::Move::Enum dir) :
        state(AMBLING),
        counter(counter),
        amble_dir(dir),
        prey(NULL),
        wait(false)
    {
    }
};


Zombie::Zombie(Map::Location loc) : 
    Entity(loc)
{
    int counter = 1 + rand() % 10;
    Map::Move::Enum amble_dir(Map::Move::Random());
    m_c = new State(counter, amble_dir);
    m_n = new State(counter, amble_dir);
    zombie_count++;
}

Zombie::~Zombie()
{
    delete m_n;
    delete m_c;
    zombie_count--;
}

int Zombie::ZombieCount()
{
    return zombie_count;
}

Map::Move::Enum Zombie::Move(const Map::Map& map)
{
    *m_n = *m_c;

    if (map.AnyEntities<Zombie>(Location(), this))
    {
        return Map::Move::Random();
    }

    Human *prey = map.NearestNeighbor<Human>(this, DETECT_RANGE);

    if (prey)
    {
        m_n->state = State::PURSUIT;
        m_n->prey = prey;
    }

    switch (m_n->state)
    {
    case State::AMBLING:
        return Ambling();
    case State::IDLE:
        return Idle();
    case State::PURSUIT:
        return Pursuit(map);
    default:
        return Map::Move::None;
    };
}

void Zombie::CommitImpl()
{
    std::swap(m_c, m_n);
}

bool Zombie::MaybeEatBrains(const Human& victim)
{
    if (&victim == m_n->prey)
    {
        m_n->prey = NULL;
        m_n->state = State::IDLE;
        m_n->counter = 3 + rand() % 10;
        return true;
    }

    return false;
}

Map::Move::Enum Zombie::Ambling()
{
    if (--m_n->counter == 0)
    {
        m_n->state = State::IDLE;
        m_n->counter = 3 + rand() % 10;

        return Map::Move::None;
    }

    return m_n->amble_dir;
}

Map::Move::Enum Zombie::Idle()
{
    if (--m_n->counter == 0)
    {
        m_n->state = State::AMBLING;
        m_n->amble_dir = Map::Move::Random();
        m_n->counter = 3 + rand() % 20;

        return m_n->amble_dir;
    }

    return Map::Move::None;
}

Map::Move::Enum Zombie::Pursuit(const Map::Map& map)
{
    int dist = Location().TaxiDistance(m_n->prey->Location());

    if (dist >= GIVE_UP_RANGE)
    {
        m_n->state = State::IDLE;
        m_n->counter = 5 + rand() % 10;
        m_n->prey = NULL;
        return Map::Move::None;
    }

    QVector2D us = Location();
    QVector2D them = m_n->prey->Location();

    Zombie *neighbor = map.NearestNeighbor<Zombie>(this, 5);
    QVector2D prey_dir = them - us;

    QVector2D move_dir = neighbor ?
        (prey_dir + -(neighbor->Location() - us)) / 2.0 : prey_dir;
    QVector2D dest = us + move_dir;

    return Location().DirectionTo(dest);
}
} // namespace Entities

} // namespace Outbreak
