#include "ui/mainwindow.h"
#include <QFileDialog>
#include <QGroupBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolBar>
#include <QPixmap>
#include <QIcon>
#include <qtoolbutton.h>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileInfo>

#include "geometry/Sphere.h"
#include "geometry/Parallelogram.h"
#include "geometry/Mesh.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){

    QGroupBox *gb = new QGroupBox(this);
    setCentralWidget(gb);
    m_gridLayout = new QGridLayout(gb);
    gb->setLayout(m_gridLayout);

    setWindowTitle("Phenix");
    resize(750,750);

    // Create our toolbar
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setOrientation(Qt::Vertical);
    m_gridLayout->addWidget(toolBar,0,0,1,1);

    // Create our path tracer
    m_pathTracer = new PathTracerScene();

    QGLFormat format;
    format.setVersion(4,1);
    format.setProfile(QGLFormat::CoreProfile);

    m_openGLWidget = new OpenGLWidget(format,this);
    //set our optix renderer
    m_openGLWidget->setOptiXRenderer(m_pathTracer);
    m_gridLayout->addWidget(m_openGLWidget,0,1,1,1);
    m_openGLWidget->show();

    // Get our screen size so we can position things properly
    QRect rec = QApplication::desktop()->screenGeometry();
    int scrnHeight = rec.height();
    int scrnWidth = rec.width();
    this->move((scrnWidth/2)-(this->width()/2),(scrnHeight/2)-(this->height()/2));

    m_inspectorMenu = new InspectorMenu(m_pathTracer,this);
    m_inspectorMenu->resize(300,this->height());
    std::cout<<"Inspector pos "<<m_inspectorMenu->pos().x()<<","<<m_inspectorMenu->pos().y()<<std::endl;
    m_inspectorMenu->move((scrnWidth/2)-(this->width()/2)-m_inspectorMenu->width(),(scrnHeight/2)-(m_inspectorMenu->height()/2));

    //Inspector button
    QPixmap inspectorImg("images/icons/inspector.png");
    QIcon inspectorBtnIcon(inspectorImg);
    QToolButton *inspectorToolbarBtn = new QToolButton(this);
    inspectorToolbarBtn->setIcon(inspectorBtnIcon);
    inspectorToolbarBtn->setToolTip("Inspector");
    toolBar->addWidget(inspectorToolbarBtn);
    toolBar->addSeparator();
    //connect our button to our show inspector slot
    connect(inspectorToolbarBtn, SIGNAL(pressed()), this, SLOT(showInspector()));

    // Play/Pause Render Button
    QPixmap ppImg("images/icons/PlayPause.png");
    QIcon ppBtnIcon(ppImg);
    QToolButton *ppToolbarBtn = new QToolButton(this);
    ppToolbarBtn->setIcon(ppBtnIcon);
    ppToolbarBtn->setToolTip("Play/Pause Rendering");
    toolBar->addWidget(ppToolbarBtn);
    toolBar->addSeparator();
    //connect our button to our show inspector slot
    connect(ppToolbarBtn, SIGNAL(pressed()), m_openGLWidget, SLOT(toggleRender()));

    //set up our menu bar
    //get problems with native menu bar so set it non native
    menuBar()->setNativeMenuBar(false);
    menuBar()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    //File toolbar tab
    QMenu *fileMenu = new QMenu("File",menuBar());
    QAction *exportRender = new QAction("Export Render",fileMenu);
    connect(exportRender,SIGNAL(triggered()),m_openGLWidget,SLOT(saveImage()));
    fileMenu->addAction(exportRender);
    menuBar()->addAction(fileMenu->menuAction());

    // Geometry toolbar tab
    QMenu *geomMenu = new QMenu("Geometry",menuBar());
    // Create sphere option
    QAction *createSphereBtn = new QAction("Create Sphere",geomMenu);
    connect(createSphereBtn,SIGNAL(triggered()),this,SLOT(createSphere()));
    geomMenu->addAction(createSphereBtn);
    menuBar()->addAction(geomMenu->menuAction());
    // Create plane option
    QAction *createPlaneBtn = new QAction("Create Plane",geomMenu);
    connect(createPlaneBtn,SIGNAL(triggered()),this,SLOT(createPlane()));
    geomMenu->addAction(createPlaneBtn);
    menuBar()->addAction(geomMenu->menuAction());
    // Import mesh
    QAction *importMeshBtn = new QAction("Import Mesh",geomMenu);
    connect(importMeshBtn,SIGNAL(triggered()),this,SLOT(importMesh()));
    geomMenu->addAction(importMeshBtn);
    menuBar()->addAction(geomMenu->menuAction());


}

MainWindow::~MainWindow(){
    delete m_pathTracer;
}

void MainWindow::createSphere()
{
    m_inspectorMenu->addGeometry(new Sphere(m_pathTracer->getContext()),"Sphere");
}

void MainWindow::createPlane()
{
    m_inspectorMenu->addGeometry(new Parallelogram(m_pathTracer->getContext()),"Plane");
}

void MainWindow::importMesh()
{
    QString dir = QFileDialog::getOpenFileName(this,"Import Mesh","./models/");
    if(!dir.isNull())
    {
        QFileInfo f(dir);
        m_inspectorMenu->addGeometry(new Mesh(dir.toStdString(),m_pathTracer->getContext()),f.fileName());
    }
}
