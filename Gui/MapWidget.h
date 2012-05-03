#ifndef OUTBREAK_MAP_WIDGET_H
#define OUTBREAK_MAP_WIDGET_H

#include <QFutureWatcher>
#include <QTimer>
#include <QWidget>

#include "Canvas.h"
#include "Entities/EntityDispatch.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Gui
{

class MapWidget : public Canvas
{
    Q_OBJECT;

public:
    MapWidget(const Map::Map& map, Entities::EntityDispatcherBase* dispatcher_p,
        QWidget *parent_p = NULL);
    MapWidget(const Map::Map& map, QWidget *parent_p = NULL );
    virtual ~MapWidget() { delete m_dispatch_p; }

    void Run();
    void Stop();

protected:
    virtual void Redraw();

private slots:
    void TimerTick();
    void StepFinished();

private:
    void Init();
    enum Status
    {
        Started,
        TimeWait,
        UpdateWait
    } m_status;

    QRectF ScalePoint(const QPoint& point);

    Entities::EntityDispatcherBase *m_dispatch_p;
    QTimer m_timer;
    QFutureWatcher<Map::Map> m_future;
    Map::Map m_map;
    int m_iteration;
};

} // namespace Gui

} // namespace Outbreak

#endif // OUTBREAK_MAP_WIDGET_H
