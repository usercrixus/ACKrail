#include "EmptyPageWidget.hpp"

EmptyPageWidget::EmptyPageWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

void EmptyPageWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.043f, 0.067f, 0.094f, 1.0f);
}

void EmptyPageWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
