#include <cstdlib>

#include "Map/Move.h"

namespace Outbreak
{

namespace Map
{

Move::Enum Move::Opposite(Enum dir)
{
    static const Enum mapping[MoveCount] = 
    { None, South, SouthWest, West, NorthWest, 
      North, NorthEast, East, SouthEast };

    return mapping[(int)dir];
}

Move::Enum Move::Random()
{
    return (Enum)(rand() % MoveCount);
}

} // namespace Move

} // namespace Map
