#include "ui/mainwindow.h"
#include <QFileDialog>
#include <QGroupBox>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){

    QGroupBox *gb = new QGroupBox(this);
    setCentralWidget(gb);
    m_gridLayout = new QGridLayout(gb);
    gb->setLayout(m_gridLayout);

    setWindowTitle("Phenix");
    resize(750,750);

    // Create our path tracer
    m_pathTracer = new PathTracerScene();

    QGLFormat format;
    format.setVersion(4,1);
    format.setProfile(QGLFormat::CoreProfile);

    m_openGLWidget = new OpenGLWidget(format,this);
    //set our optix renderer
    m_openGLWidget->setOptiXRenderer(m_pathTracer);
    m_gridLayout->addWidget(m_openGLWidget,0,0,1,1);
    m_openGLWidget->show();
}

MainWindow::~MainWindow(){
    delete m_pathTracer;
}
