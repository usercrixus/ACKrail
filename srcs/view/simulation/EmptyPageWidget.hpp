#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class EmptyPageWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    explicit EmptyPageWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
};
