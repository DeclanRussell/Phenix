#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
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





};

#endif // MAINWINDOW_H
