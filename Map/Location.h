#ifndef OUTBREAK_MAP_LOCATION_H
#define OUTBREAK_MAP_LOCATION_H

#include <QPoint>

#include "Map/Move.h"

namespace Outbreak
{

namespace Map
{

struct Location
{
    Location(int x, int y) : X(x), Y(y) { }

    Location Neighbor(Move::Enum dir) const;

    Move::Enum DirectionTo(Location loc) const;
    int TaxiDistance(Location loc) const;

    operator QPoint() { return QPoint(X, Y); }

    int X;
    int Y;
};

inline bool operator==(const Location p, const Location q)
{
    return (p.X == q.X) && (p.Y == q.Y);
}

inline uint qHash(const Location& loc)
{
    const size_t halfwidth = (sizeof(uint) / 2) * 8;
    const size_t mask = (1 << halfwidth) - 1;
    uint low_x = loc.X & mask;
    uint low_y = loc.Y & mask;

    return (low_x << halfwidth) | low_y;
}

} // namespace Map

} // namespace Outbreak

#endif // OUTBREAK_MAP_LOCATION_H
