#ifndef OUTBREAK_MAP_WIDGET_H
#define OUTBREAK_MAP_WIDGET_H

#include <QFutureWatcher>
#include <QGLWidget>
#include <QMatrix4x4>
#include <QVector2D>
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
    virtual void keyPressEvent(QKeyEvent *event_p);
    virtual void wheelEvent(QWheelEvent *event_p);
    virtual void mousePressEvent(QMouseEvent *event_p);
    virtual void mouseMoveEvent(QMouseEvent *event_p);

private slots:
    void TimerTick();
    void StepFinished();

private:
    void Init();
    void DrawBorder();
    void DrawEntities();
    void Vertex(const QVector3D &v);
    void Vertex(const Map::Location& loc);
    void Vertex(double x, double y, double z = 0);
    Map::Location MapCoord(const QPoint& pos) const;
    QPoint MapCoord(const Map::Location& loc) const;

    enum Status
    {
        Started,
        TimeWait,
        UpdateWait
    } m_status;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_modelview;
    QVector2D m_delta_center;

    Entities::EntityDispatcherBase *m_dispatch_p;
    QTimer m_timer;
    QFutureWatcher<Map::Map> m_future;
    Map::Map m_map;
    int m_iteration;
    double m_zoom;
    Map::Location m_center;
};

} // namespace Gui

} // namespace Outbreak

#endif // OUTBREAK_MAP_WIDGET_H
