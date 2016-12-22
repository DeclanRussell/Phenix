#include "ui/GeometryAttribEditor.h"
#include <QLabel>
#include <iostream>

GeometryAttribEditor::GeometryAttribEditor(AbstractOptixRenderer *_renderer, QWidget *parent):QWidget(parent,Qt::Window),m_renderer(_renderer)
{
    m_geometry = 0;
    setWindowTitle("Attribute Editor");
    // Create our widget layout
    m_gridLayout = new QGridLayout(this);
    this->setLayout(m_gridLayout);

    // Add position field
    m_gridLayout->addWidget(new QLabel("Position"),0,0,1,1);
    m_posX = new QDoubleSpinBox(this);
    connect(m_posX,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_posX->setValue(0.0);
    m_posX->setMaximum(-INFINITY);
    m_posX->setMaximum(INFINITY);
    m_posX->setDecimals(3);
    m_gridLayout->addWidget(m_posX,0,1,1,1);
    m_posY = new QDoubleSpinBox(this);
    connect(m_posY,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_posY->setValue(0.0);
    m_posY->setMaximum(-INFINITY);
    m_posY->setMaximum(INFINITY);
    m_posY->setDecimals(3);
    m_gridLayout->addWidget(m_posY,0,2,1,1);
    m_posZ = new QDoubleSpinBox(this);
    connect(m_posZ,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_posZ->setValue(0.0);
    m_posZ->setMaximum(-INFINITY);
    m_posZ->setMaximum(INFINITY);
    m_posZ->setDecimals(3);
    m_gridLayout->addWidget(m_posZ,0,3,1,1);

    // Add rotation field
    m_gridLayout->addWidget(new QLabel("Rotation"),1,0,1,1);
    m_rotX = new QDoubleSpinBox(this);
    connect(m_rotX,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_rotX->setValue(0.0);
    m_rotX->setMaximum(-INFINITY);
    m_rotX->setMaximum(INFINITY);
    m_rotX->setDecimals(3);
    m_gridLayout->addWidget(m_rotX,1,1,1,1);
    m_rotY = new QDoubleSpinBox(this);
    connect(m_rotY,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_rotY->setValue(0.0);
    m_rotY->setMaximum(-INFINITY);
    m_rotY->setMaximum(INFINITY);
    m_rotY->setDecimals(3);
    m_gridLayout->addWidget(m_rotY,1,2,1,1);
    m_rotZ = new QDoubleSpinBox(this);
    connect(m_rotZ,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_rotZ->setValue(0.0);
    m_rotZ->setMaximum(-INFINITY);
    m_rotZ->setMaximum(INFINITY);
    m_rotZ->setDecimals(3);
    m_gridLayout->addWidget(m_rotZ,1,3,1,1);

    // Add scale field
    m_gridLayout->addWidget(new QLabel("Scale"),2,0,1,1);
    m_scaleX = new QDoubleSpinBox(this);
    connect(m_scaleX,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_scaleX->setValue(1.0);
    m_scaleX->setMaximum(-INFINITY);
    m_scaleX->setMaximum(INFINITY);
    m_scaleX->setDecimals(3);
    m_gridLayout->addWidget(m_scaleX,2,1,1,1);
    m_scaleY = new QDoubleSpinBox(this);
    connect(m_scaleY,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_scaleY->setValue(1.0);
    m_scaleY->setMaximum(-INFINITY);
    m_scaleY->setMaximum(INFINITY);
    m_scaleY->setDecimals(3);
    m_gridLayout->addWidget(m_scaleY,2,2,1,1);
    m_scaleZ = new QDoubleSpinBox(this);
    connect(m_scaleZ,SIGNAL(valueChanged(double)), this,SLOT(updateAttributes(double)));
    m_scaleZ->setValue(1.0);
    m_scaleZ->setMaximum(-INFINITY);
    m_scaleZ->setMaximum(INFINITY);
    m_scaleZ->setDecimals(3);
    m_gridLayout->addWidget(m_scaleZ,2,3,1,1);
}
//----------------------------------------------------------------------------------------------------------------------
void GeometryAttribEditor::setCurrentGeometry(AbstractOptixGeometry *_geo)
{
    // Set our local pointer for later use
    m_geometry = _geo;

    // Get the attributes from the geometry
    optix::float3 pos;
    optix::float3 rot;
    optix::float3 scale;
    _geo->getPos(pos.x,pos.y,pos.z);
    _geo->getRot(rot.x,rot.y,rot.z);
    _geo->getScale(scale.x,scale.y,scale.z);

    // Set the attributes
    m_posX->setValue(pos.x);
    m_posY->setValue(pos.y);
    m_posZ->setValue(pos.z);
    m_rotX->setValue(rot.x);
    m_rotY->setValue(rot.y);
    m_rotZ->setValue(rot.z);
    m_scaleX->setValue(scale.x);
    m_scaleY->setValue(scale.y);
    m_scaleZ->setValue(scale.z);

}
//----------------------------------------------------------------------------------------------------------------------
void GeometryAttribEditor::updateAttributes(double)
{
    if(!m_geometry)
    {
        std::cerr<<"Attribute editor geometry pointer does not point to anything"<<std::endl;
        return;
    }
    if(!m_renderer)
    {
        std::cerr<<"Attribute editor renderer pointer does not point to anything"<<std::endl;
        return;
    }
    m_geometry->setPos(m_posX->value(),m_posY->value(),m_posZ->value());
    m_geometry->setRot(m_rotX->value(),m_rotY->value(),m_rotZ->value());
    m_geometry->setScale(m_scaleX->value(),m_scaleY->value(),m_scaleZ->value());
    m_renderer->rebuildScene();
}
//----------------------------------------------------------------------------------------------------------------------
