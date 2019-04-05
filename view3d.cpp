#include "view3d.h"

#include <QFile>
#include <QCoreApplication>
#include <QVector>

static const float vertices[] = {
     .0f,   .5f, -1.0f,
     .5f, -.5f, -1.0f,
    -.5f, -.5f, -1.0f
};

void View3D::initializeGL() {
    initializeOpenGLFunctions();
    //connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
    printContextInformation();

    // Create Shader (Do not release until VAO is created)
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/VertexShader.glsl");

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/FragmentShader.glsl");

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", 0);
    if (!program->link())
        qDebug()<<"Link error!\n";
    program->bind();

    // Create Buffer (Do not release until VAO is created)
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertices, 3*3*sizeof(float));

    // Create Vertex Array Object
    vao.create();
    vao.bind();
    int attributelocation = program->attributeLocation("vertex");
    program->enableAttributeArray(attributelocation);
    program->setAttributeBuffer(attributelocation, GL_FLOAT, 0, 3);

    program->setUniformValue("vertexcolor", .2f, .1f, .1f);

    vao.release();
    vbo.release();
    program->release();

    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void View3D::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    program->setUniformValue("MVP", pMatrix);

    vao.bind();

    glDrawArrays(GL_TRIANGLES, 0, 3);

    vao.release();

    program->release();

/*
    QPainter qPainter(this);
    QPen qPen;
    qPen.setWidth(1);
    qPen.setColor(QColor(Qt::black));
    qPen.setStyle(Qt::SolidLine);
    qPainter.resetMatrix();
    qPainter.setPen(qPen);
    qPainter.drawLine(0, 0, width(), height());
    qPainter.drawLine(0, height(), width(), 0);
*/
}

void View3D::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    //glViewport(0, 0, width, height);

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0,  width / static_cast<float>(height), 0.001f, 1000);
}

void View3D::teardownGL() {
    vao.destroy();
    vbo.destroy();
    delete program;
}

void View3D::printContextInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
