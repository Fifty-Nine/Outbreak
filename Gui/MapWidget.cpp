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
    Canvas(parent_p), 
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
    Canvas(parent_p), 
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

QRectF MapWidget::ScalePoint(const QPoint& loc)
{
    qreal x = loc.x();
    qreal y = loc.y();

    qreal xscale = width() / (qreal)m_map.Width();
    qreal yscale = height() / (qreal)m_map.Height();

    return QRectF(x * xscale, y * yscale, xscale, yscale);
}

void MapWidget::Redraw()
{
    glLoadIdentity();
    glTranslatef( -width() / 2.0f, -height() / 2.0f, 0.0f );

    for (QHash<Map::Location, Entities::Entity*>::const_iterator it = 
            m_map.Entities().begin(); 
         it != m_map.Entities().end(); ++it)
    {
        Entities::Entity* entity_p = it.value();
        qglColor( entity_p->Color() );
        Draw( entity_p->Location() );
        //painter.setPen(entity_p->Color());
        //painter.drawPoint(entity_p->Location());
        //painter.fillRect(ScalePoint(entity_p->Location()), entity_p->Color());
    }

    QPainter painter( this );
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(0, 10, QString::number(m_iteration));
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
