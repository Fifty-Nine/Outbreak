#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>

class Canvas : public QGLWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);

public slots:
    void Zoom( float z );

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();

    virtual void Redraw() = 0;

    void AddVertex( const QPoint& pt );
    void AddVertex( const QPointF& pt );
    void AddVertex( const QVector2D& v );
    void Draw( const QPoint& pt, float size = 1.0f );
    void Draw( const QPointF& pt, float size = 1.0f );
    void Draw( const QVector2D& v, float size = 1.0f );
    void Draw( const QPolygonF& poly, GLenum mode = GL_LINE_LOOP );

private:
    float m_zoom;
};

#endif // CANVAS_H
