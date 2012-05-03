#include <QPainter>
#include <QtConcurrentRun>

#include <boost/bind.hpp>

#include "Entities/Entity.h"
#include "Map/Map.h"
#include "Gui/MapWidget.h"

namespace 
{

const int UPDATE_PERIOD = 20;

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
    m_map(map)
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
    m_map(map)
{
    Init();
}

void MapWidget::Run()
{
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

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(
        0.0f, (float)m_map.Width(), 0.0f, (float)m_map.Height(), -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );

    update();
}

void MapWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glBegin( GL_POINTS );

    foreach ( Entities::Entity *entity_p, m_map.Entities() )
    {
        qglColor( entity_p->Color() );
        glVertex2i( entity_p->Location().X, entity_p->Location().Y );
    }

    glEnd();
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

} // namespace Gui

} //namespace Outbreak
