#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "renderer/PathTracer.h"
#include "ui/OpenGLWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    OpenGLWidget *m_openGLWidget;
    QGridLayout *m_gridLayout;
    PathTracerScene *m_pathTracer;





};

#endif // MAINWINDOW_H
