#include <QApplication>
#include <QDebug>

#include <ctime>
#include <cstdlib>

#include "Entities/Brownian.h"
#include "Entities/Firefly.h"
#include "Entities/Flocker.h"
#include "Entities/Human.h"
#include "Entities/Zombie.h"
#include "Gui/MapWidget.h"

using namespace Outbreak;
using namespace Outbreak::Entities;

Map::Location RandomLocation(int w, int h)
{
    return Map::Location(rand() % w, rand() % h);
}

void MakeFlock(QVector<Entity*>& res, int count, int w, int h)
{
    res.append(new Flocker(RandomLocation(w, h), true));

    for (int i = 1; i < count; i++)
    {
        res.append(new Flocker(RandomLocation(w, h), true));
    }
}

int main(int argc, char ** argv)
{
    srand(time(0));

    QApplication app(argc, argv);

    int width = 1600;
    int height = 900;

    QVector<Entity*> entities;

    for (int i = 0; i < 0; i++)
    {
        int x = rand() % width;
        int y = rand() % height;

        Brownian *entity_p = new Brownian(Map::Location(x,y));
        entities.append(entity_p);
    }

    for (int i = 0; i < 00; i++)
    {
        int x = rand() % width;
        int y = rand() % height;
        entities.append(new Human(Map::Location(x, y)));
    }

    for (int i = 0; i < 0; i++)
    {
        int x = rand() % width;
        int y = rand() % height;
        entities.append(new Zombie(Map::Location(x, y)));
    }

    int x;
    int y;
    for (int i = 0; i < 2000; i++)
    {
        int x = rand() % width;
        int y = rand() % height;
        entities.append(new Firefly(Map::Location(x, y)));
    }

    for (int i = 0; i < 2000; i++)
    {
        x = rand() % width;
        y = rand() % height;
        entities.append(new Firefly(Map::Location(x, y)));
    }

    x = rand() % width;
    y = rand() % height;
    entities.append(new Firefly(Map::Location(x, y), true));
    //MakeFlock(entities, 256, width, height);
  
    qDebug() << entities.count();
    Map::Map map(width, height, entities);

    Gui::MapWidget widget(map);

    widget.show();

    widget.resize(QSize(width, height));
    widget.Run();

    int rc = app.exec();

    qDebug() << Zombie::ZombieCount();

    return rc;
}
