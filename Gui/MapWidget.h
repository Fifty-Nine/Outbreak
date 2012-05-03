#ifndef OUTBREAK_MAP_WIDGET_H
#define OUTBREAK_MAP_WIDGET_H

#include <QFutureWatcher>
#include <QGLWidget>
#include <QTimer>

#include "Entities/EntityDispatch.h"
#include "Map/Map.h"

namespace Outbreak
{

namespace Gui
{

class MapWidget : public QGLWidget
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
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

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

    Entities::EntityDispatcherBase *m_dispatch_p;
    QTimer m_timer;
    QFutureWatcher<Map::Map> m_future;
    Map::Map m_map;
    int m_iteration;
};

} // namespace Gui

} // namespace Outbreak

#endif // OUTBREAK_MAP_WIDGET_H
