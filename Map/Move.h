#ifndef OUTBREAK_MAP_MOVE_H
#define OUTBREAK_MAP_MOVE_H

namespace Outbreak
{

namespace Map
{

namespace Move
{

enum Enum
{
    None,
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
    MoveCount
};

Enum Opposite(Enum dir);

Enum Random();

} // namespace Move

} // namespace Map

} // namespace Outbreak

#endif // OUTBREAK_MAP_MOVE_H
