#include "Brownian.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Entities 
{

Brownian::Brownian(Map::Location location) : 
    Entity(location) 
{
}

Map::Move::Enum Brownian::Move(const Map::Map& map)
{
    (void)map;

    return Map::Move::Random();
}

} //namespace Entities

} //namespace Outbreak
