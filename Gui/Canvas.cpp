#define NOMINMAX

#include <algorithm>

#include <QVector2D>

#include "Canvas.h"

Canvas::Canvas(QWidget *parent) :
    QGLWidget(parent), 
    m_zoom( 1.0f )
{
}

void Canvas::Zoom( float z )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    float side = std::min( width(), height() ) * z / 2.0f;
    glOrtho( -side, side, -side, side, 0.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );

    update();
}

void Canvas::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glShadeModel( GL_SMOOTH );
}

void Canvas::resizeGL(int w, int h)
{
    int side = std::min( w, h );

    glViewport( (w - side) / 2, (h - side) / 2, side, side );

    Zoom( 1.0f );
}

void Canvas::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    qglColor( Qt::red );
    glPointSize( 10.0f );

    Redraw();
}

void Canvas::AddVertex( const QPoint& p )
{
    glVertex2i( p.x(), p.y() );
}

void Canvas::AddVertex( const QPointF& p )
{
    glVertex2f( p.x(), p.y() );
}

void Canvas::AddVertex( const QVector2D& v )
{
    AddVertex( v.toPointF() );
}

void Canvas::Draw( const QPoint& pt, float size )
{
    glPointSize( size );
    glBegin( GL_POINTS ); 
    AddVertex( pt );
    glEnd();
}

void Canvas::Draw( const QPointF& pt, float size )
{
    glPointSize( size );
    glBegin( GL_POINTS );
    AddVertex( pt );
    glEnd();
}

void Canvas::Draw( const QVector2D& v, float size )
{
    Draw( v.toPointF(), size );
}

void Canvas::Draw( const QPolygonF& poly, GLenum mode )
{
    glBegin( mode );

    foreach ( const QPointF& p, poly )
    {
        AddVertex( p );
    }

    glEnd();
}
