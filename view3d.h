#ifndef VIEW3D_H
#define VIEW3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class View3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    View3D(QWidget *parent = nullptr) : QOpenGLWidget(parent) {}

public:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);

    virtual ~View3D() { teardownGL(); }

protected slots:
    virtual void teardownGL();

private:
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;

    QMatrix4x4 pMatrix;

    void printContextInformation();
};

#endif // VIEW3D_H
