#ifndef OUTBREAK_ENTITY_DISPATCH_H
#define OUTBREAK_ENTITY_DISPATCH_H

#include <QList>

#include "Util/Dispatcher.h"

#include "Entities/Entity.h"

namespace Outbreak
{

namespace Entities
{

class Human;
class Zombie;

typedef Util::Dispatcher<Entity, Entity, QList<Entity*> > EntityDispatcherBase; 

class DefaultDispatcher : public EntityDispatcherBase
{
public: 
    DefaultDispatcher();

    static QList<Entity*> Interact(Zombie& z, Human& h);
    static QList<Entity*> Interact(Entity& e1, Entity& e2);
};

} // namespace Entities

} // namespace Outbreak

#endif // OUTBREAK_ENTITY_DISPATCH_H

