#ifdef WIN32
# define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>

#include "Map/Location.h"

namespace Outbreak
{

namespace Map
{

Location Location::Neighbor(Move::Enum dir) const
{
    Location result(X, Y);

    switch (dir)
    {
    case Move::North:
        result.Y += 1;
        break;
    case Move::NorthEast:
        result.X += 1;
        result.Y += 1;
        break;
    case Move::East:
        result.X += 1;
        break;
    case Move::SouthEast:
        result.X += 1;
        result.Y -= 1;
        break;
    case Move::South:
        result.Y -= 1;
        break;
    case Move::SouthWest:
        result.X -= 1;
        result.Y -= 1;
        break;
    case Move::West:
        result.X -= 1;
        break;
    case Move::NorthWest:
        result.X -= 1;
        result.Y += 1;
        break;
    default:
        break;
    }

    return result;
}

Move::Enum Location::DirectionTo(Location q) const
{
    const Location& p( *this );
    double phi = atan2((double)q.Y - p.Y, q.X - p.X);

    const double pi_8 = M_PI / 8.0;

    if (phi < 0.0)
    {
        phi += 2.0 * M_PI;
    }

    return (phi < ( 1*pi_8)) ? Move::East      :
           (phi < ( 3*pi_8)) ? Move::NorthEast :
           (phi < ( 5*pi_8)) ? Move::North     :
           (phi < ( 7*pi_8)) ? Move::NorthWest :
           (phi < ( 9*pi_8)) ? Move::West      :
           (phi < (11*pi_8)) ? Move::SouthWest :
           (phi < (13*pi_8)) ? Move::South     :
           (phi < (15*pi_8)) ? Move::SouthEast :
           (phi < (17*pi_8)) ? Move::East      :
                               Move::None      ;
}

int Location::TaxiDistance(Location q) const
{
    const Location& p( *this );

    return (abs(q.X - p.X) + abs(q.Y - p.Y));
}

} // namespace Map

} // namespace Outbreak
