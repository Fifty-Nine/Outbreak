#include <QtOpenGl>
#include <GL/glu.h>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QtConcurrentRun>

#include <boost/bind.hpp>

#include "Entities/Entity.h"
#include "Map/Map.h"
#include "Gui/MapWidget.h"

namespace 
{
using namespace Outbreak;

/// The number of milliseconds between updates.
const int UPDATE_PERIOD = 20;

/// The near-field clipping value.
const double NEAR_VAL = -1.0;

/// The far-field clipping value.
const double FAR_VAL = 1.0;

/// The maximum pan speed per tick, in map units.
const double PAN_SPEED = 0.01;

/** Clamp a value between two values.
  * \param[in] low The inclusive minimum value.
  * \param[in] high The inclusive maximum value.
  * \param[in] value The value to clamp.
  * \return The value, set to \c low if it was less than \c low,
  *     \c high if it was greater than \c high or \c value otherwise.
  */
template<class T> T Clamp( T low, T high, T value )
{
    return ( value < low )  ?   low :
           ( value > high ) ?  high :
                              value ;

}

/** Clamp a map location within the bounds of the given map.
  * \param[in] map The map to clamp the location to.
  * \param[in] loc The location to clamp.
  * \return The clamped location.
  */
Map::Location Clamp(const Map::Map& map, const Map::Location& loc)
{
    Map::Location r(0, 0);
    r.X = Clamp(0, map.Width(), loc.X);
    r.Y = Clamp(0, map.Height(), loc.Y);
    return r;
}

/** Clamp a vector within the given limits. Values are clamped on an
  * axis-by-axis basis.
  * \param[in] min The minimum allowable value.
  * \param[in] max The maximum allowable value.
  * \param[in] value The value to clamp.
  * \return The clamped value.
  */
QVector2D Clamp(
    const QVector2D& min, const QVector2D& max, const QVector2D& vec)
{
    return QVector2D(
        Clamp(min.x(), max.x(), vec.x()),
        Clamp(min.y(), max.y(), vec.y()));
}

/** Construct a transformation matrix that centers the given location
  * in the viewport and scales the map by the given zoom factor.
  * \param[in] map The map we're looking at.
  * \param[in] center The point to center.
  * \param[in] zoom The scale factor.
  * \return The transformation matrix.
  */
QMatrix4x4 PanAndZoom(
    const Map::Map& map, const Map::Location& center, double zoom)
{
    QMatrix4x4 result;
    result.translate(
        map.Width() * (1.0 - zoom) / 2.0,
        map.Height() * (1.0 - zoom) / 2.0 );
    result.scale( zoom );
    result.translate(
        map.Width() / 2.0 - center.X,
        map.Height() / 2.0 - center.Y );

    return result;
}

}

namespace Outbreak
{

namespace Gui
{

void MapWidget::Init()
{
    m_iteration = 0;
    setAutoFillBackground(false);
    m_timer.setInterval(UPDATE_PERIOD);
    m_timer.setSingleShot(true);

    (void)connect(&m_timer, SIGNAL(timeout()), SLOT(TimerTick()));
    (void)connect(&m_future, SIGNAL(finished()), SLOT(StepFinished()));
}

MapWidget::MapWidget(const Map::Map& map, QWidget *parent_p) : 
    QGLWidget(parent_p),
    m_status(Started),
    m_dispatch_p(new Entities::DefaultDispatcher),
    m_timer(this), 
    m_future(this), 
    m_map(map),
    m_zoom(1.0),
    m_center(map.Width() / 2, map.Height() / 2)
{
    Init();
}

MapWidget::MapWidget(const Map::Map& map, 
                     Entities::EntityDispatcherBase *dispatch_p, 
                     QWidget *parent_p) : 
    QGLWidget(parent_p),
    m_status(Started),
    m_dispatch_p(dispatch_p),
    m_timer(this), 
    m_future(this), 
    m_map(map),
    m_zoom(1.0),
    m_center(map.Width() / 2, map.Height() / 2)
{
    Init();
}

void MapWidget::Run()
{
    m_center.X += m_delta_center.x();
    m_center.Y += m_delta_center.y();
    m_center = Clamp(m_map, m_center);

    m_modelview = PanAndZoom(m_map, m_center, 1.0 / m_zoom);
    m_delta_center = QVector2D(0, 0);

    m_status = Started;
    m_future.setFuture(QtConcurrent::run(
        boost::bind(&Map::Map::Iterate, m_map, *m_dispatch_p)));
    m_timer.start();
}

void MapWidget::Stop()
{
    m_timer.stop();
}

void MapWidget::initializeGL()
{
    qglClearColor(Qt::black);
}

void MapWidget::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, h );

    m_projection.setToIdentity();
    m_projection.ortho(
        0.0, m_map.Width(),
        0.0, m_map.Height(),
        NEAR_VAL, FAR_VAL);

    m_modelview = PanAndZoom(m_map, m_center, 1.0 / m_zoom);

    update();
}

void MapWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );

    DrawBorder();
    DrawEntities();

    glBegin(GL_POINTS);
    qglColor(Qt::white);
    Vertex(m_center);
    glEnd();
}

void MapWidget::keyPressEvent(QKeyEvent *event_p)
{
    bool is_escape = ( event_p->key() == Qt::Key_Escape );
    bool alt_pressed = ( event_p->modifiers() & Qt::AltModifier );
    bool is_alt_enter = alt_pressed && ( event_p->key() == Qt::Key_Return );

    if ( is_escape && isFullScreen() )
    {
        setWindowState( windowState() & ~Qt::WindowFullScreen );
    }
    else if ( is_escape && !isFullScreen() )
    {
        close();
    }
    else if ( is_alt_enter )
    {
        setWindowState( windowState() ^ Qt::WindowFullScreen );
    }
}

void MapWidget::wheelEvent(QWheelEvent *event_p)
{
    double degrees = -event_p->delta() / 8.0;
    double steps = degrees / 15.0;
    double change = steps * 0.1;

    m_zoom = Clamp( 0.1, 10.0, m_zoom + change );
    m_modelview = PanAndZoom(m_map, m_center, 1.0 / m_zoom);
}

void MapWidget::mousePressEvent(QMouseEvent *event_p)
{
    m_center = Clamp(m_map, MapCoord(event_p->pos()));
    m_modelview = PanAndZoom(m_map, m_center, 1.0 / m_zoom);
}

void MapWidget::mouseMoveEvent(QMouseEvent *event_p)
{
    Map::Location new_center = MapCoord(event_p->pos());
    const QVector2D delta_clamp =
        PAN_SPEED * QVector2D(m_map.Width(), m_map.Height()) * m_zoom;
    m_delta_center = Clamp(-delta_clamp, delta_clamp,
        QVector2D(
            new_center.X - m_center.X,
            new_center.Y - m_center.Y));
}

void MapWidget::TimerTick()
{
    if (m_status == TimeWait)
    {
        Run();
    }
    else if (m_status == Started)
    {
        m_status = UpdateWait;
    }
}

void MapWidget::StepFinished()
{
    m_map = m_future.result();
    m_iteration++;
    
    if (m_status == UpdateWait)
    {
        Run();
    }
    else if (m_status == Started)
    {
        m_status = TimeWait;
    }

    update();
}

void MapWidget::DrawBorder()
{
    glColor4f(0.0f, 0.1f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    {
        Vertex(-1, -1);
        Vertex(m_map.Width() + 1, -1);
        Vertex(m_map.Width() + 1, m_map.Height() + 1);
        Vertex(-1, m_map.Height() + 1);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    {
        qglColor( Qt::red );
        Vertex(-1, -1);
        Vertex(m_map.Width() + 1, -1);
        Vertex(m_map.Width() + 1, m_map.Height() + 1);
        Vertex(-1, m_map.Height() + 1);
    }
    glEnd();
}

void MapWidget::DrawEntities()
{
    glBegin(GL_QUADS);
    foreach (Entities::Entity *entity_p, m_map.Entities())
    {
        Map::Location loc(entity_p->Location());
        qglColor(entity_p->Color());
        Vertex(loc.X + 1, loc.Y);
        Vertex(loc.X, loc.Y + 1);
        Vertex(loc.X - 1, loc.Y);
        Vertex(loc.X, loc.Y - 1);
    }
    glEnd();

    glBegin(GL_POINTS);
    glPointSize(1.0);
    foreach (Entities::Entity *entity_p, m_map.Entities())
    {
        qglColor(entity_p->Color());
        Vertex(entity_p->Location());
    }
    glEnd();
}

void MapWidget::Vertex(const QVector3D& wc)
{
    QVector3D ec = m_modelview.map(wc);
    QVector3D cc = m_projection.map(ec);
    glVertex3d( cc.x(), cc.y(), cc.z() );
}

void MapWidget::Vertex(const Map::Location& loc)
{
    Vertex(QVector3D(loc.X, loc.Y, 0));
}

void MapWidget::Vertex( double x, double y, double z )
{
    Vertex(QVector3D(x, y, z));
}

Map::Location MapWidget::MapCoord(const QPoint& pos) const
{
    const QVector4D screen(pos.x(), height() - pos.y(), 0.0, 1.0);
    const QVector4D normalized = QVector4D(
        screen.x() / width(),
        screen.y() / height(),
        (screen.z() - NEAR_VAL) / (FAR_VAL - NEAR_VAL),
        0.5 * screen.w() ) * 2.0 -
        QVector4D(1.0, 1.0, 1.0, 0.0);
    const QVector4D clip = normalized * normalized.w();
    const QVector4D eye = m_projection.inverted().map(clip);
    const QVector4D world = m_modelview.inverted().map(eye);

    return Map::Location(world.x(), world.y());
}

QPoint MapWidget::MapCoord(const Map::Location& pos) const
{
    const QVector4D world(pos.X, pos.Y, 0.0, 1.0);
    const QVector4D eye = m_modelview.map(world);
    const QVector4D clip = m_projection.map(eye);
    const QVector3D normalized =
        0.5 * clip.toVector3DAffine() + QVector3D(0.5, 0.5, 0.5);
    const QPoint screen(
        normalized.x() * width(),
        height() - normalized.y() * height() );

    return screen;
}

} // namespace Gui

} //namespace Outbreak
