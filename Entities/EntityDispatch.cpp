#include <QDebug>

#include "Entities/EntityDispatch.h"
#include "Entities/Human.h"
#include "Entities/Zombie.h"

namespace Outbreak
{

namespace Entities
{

DefaultDispatcher::DefaultDispatcher()
{
    Add<Zombie, Human, Interact>();
    Add<Entity, Entity, Interact>();
}

QList<Entity*> DefaultDispatcher::Interact(Zombie& z, Human& h)
{
    assert(z.Alive() && h.Alive());

    // Zombies assimilate humans.
    QList<Entity*> result;

    if (z.MaybeEatBrains(h))
    {
        Zombie *assimilated_p = new Zombie(h.Location());
        h.Kill();

        result.append(assimilated_p);
    }

    return result;
}

QList<Entity*> DefaultDispatcher::Interact(Entity& e1, Entity& e2)
{
    assert(e1.Alive() && e2.Alive());

    // Do nothing in the default case.
    return QList<Entity*>();
}

} // namespace Entities

} // namespace Outbreak
