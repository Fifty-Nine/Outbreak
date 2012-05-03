#include <QDebug>

#include "Entities/Firefly.h"
#include "Map/Map.h"

#define FIREFLY_DEBUG(stuff)\
    if (m_debug) qDebug() << "Firefly" << (uintptr_t)this << "-" << stuff

namespace
{
const int COLOR_CHANGE_TIME = 50;
const int SCAN_DISTANCE = 15;


static const QColor colors[] = 
{
    QColor(255, 0, 0),
    QColor(0, 255, 0),
    QColor(0, 0, 255),
    QColor(255, 255, 0),
    QColor(255, 0, 255),
    QColor(0, 255, 255),
    QColor(255, 255, 255)
};
const size_t color_count = sizeof(colors) / sizeof(QColor);

}

namespace Outbreak
{
    
namespace Entities
{

class Firefly::State
{
public:
    Firefly *source_p;
    Map::Location dest;
    int color;
    int counter;
    int version;

    State(Firefly *parent_p, Map::Location location, 
          int color, int counter) : 
        source_p(parent_p), 
        dest(location),
        color(color), 
        counter(counter),
        version(0)
    {
    }
};

Firefly::Firefly(Map::Location location, bool debug) : 
    Entity(location),
    m_debug(debug)
{
    int color = rand() % color_count;
    int counter = rand() % COLOR_CHANGE_TIME;
    m_c = new State(this, location, color, counter);
    m_n = new State(this, location, color, counter);
}

Firefly::~Firefly()
{
    delete m_c;
    delete m_n;
}

Map::Move::Enum Firefly::Move(const Map::Map& map)
{
    *m_n = *m_c;
    if (m_n->dest == Location())
    {
        m_n->dest = map.RandomLocation();
    }

    Firefly *nearby_p = map.NearestNeighbor<Firefly>(this, SCAN_DISTANCE);
    
    if (nearby_p) ExamineNeighbor(nearby_p);

    if (--(m_n->counter) <= 0)
    {
        NextColor();
        m_n->counter = COLOR_CHANGE_TIME;
        ++m_n->version;
        m_n->source_p = this;
        FIREFLY_DEBUG("Source:" << m_n->source_p << "Version:" << m_n->version 
                                << "Color:" << colors[m_n->color] 
                                << "Counter:" << m_n->counter);
    }

    return Location().DirectionTo(m_n->dest);
}

void Firefly::CommitImpl()
{
    std::swap(m_c, m_n);
}

QColor Firefly::Color() 
{
    return colors[m_c->color];
}

void Firefly::ExamineNeighbor(Firefly *neighbor_p)
{
    uintptr_t us = (uintptr_t)m_n->source_p;
    uintptr_t them = (uintptr_t)neighbor_p->m_c->source_p;
    int their_ver = neighbor_p->m_c->version;
    int our_ver = m_n->version;

    if (them > us || ((them == us) && (their_ver > our_ver)))
    {
        m_n->color = neighbor_p->m_c->color;
        m_n->counter = neighbor_p->m_c->counter + (rand() % 3 - 1);
        m_n->source_p = neighbor_p->m_c->source_p;
        m_n->version = neighbor_p->m_c->version;
        FIREFLY_DEBUG("Source:" << m_n->source_p << "Version:" << m_n->version 
                   << "Color:" << colors[m_n->color] 
                   << "Counter:" << m_n->counter);
    }
}

void Firefly::NextColor()
{
    m_n->color++; 
    m_n->color %= color_count;
}

} // namespace Entities

} // namespace Outbreak
