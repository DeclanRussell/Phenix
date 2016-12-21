#include "ui/InspectorMenu.h"
#include <iostream>
#include <QPushButton>
#include <QInputDialog>

InspectorMenu::InspectorMenu(AbstractOptixRenderer *_renderer, QWidget *_parent) : QWidget(_parent,Qt::Window)
{
    if(_renderer)
    {
        m_renderer = _renderer;
    }
    else
    {
        std::cerr<<"No renderer set. Cannot create inspector menu. Exiting!"<<std::endl;
        exit(-1);
    }

    setWindowTitle("Inspector");

    // Create our widget layout
    m_gridLayout = new QGridLayout(this);
    this->setLayout(m_gridLayout);

    // Create our list widget
    m_sceneList = new QListWidget(this);
    m_gridLayout->addWidget(m_sceneList,0,0,4,3);

    QPushButton *renameItemBtn = new QPushButton("Rename",this);
    m_gridLayout->addWidget(renameItemBtn,4,1,1,1);
    connect(renameItemBtn,SIGNAL(pressed()),this,SLOT(renameSelected()));

    QPushButton *removeItemBtn = new QPushButton("Remove",this);
    m_gridLayout->addWidget(removeItemBtn,4,2,1,1);
    connect(removeItemBtn,SIGNAL(pressed()),this,SLOT(removeSelected()));

}
//----------------------------------------------------------------------------------------------------------------------
InspectorMenu::~InspectorMenu()
{
    //select all items in list
    m_sceneList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_sceneList->selectAll();
    //delete all that are selected
    removeSelected();
    m_sceneList->setSelectionMode(QAbstractItemView::SingleSelection);
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::removeSelected()
{
    QList<QListWidgetItem*> items = m_sceneList->selectedItems();
    for(int i=0;i<items.size();i++)
    {
        removeItem(items[i]);
    }
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::removeItem(QListWidgetItem *_item)
{
    if(!_item)
    {
        std::cerr<<"Nothing to remove"<<std::endl;
        return;
    }
    AbstractOptixObject *data = ((OptixQListWidgetItem*)_item)->getData();
    switch (_item->type()) {
    case OptixQListWidgetItem::Geometry:
    {
        removeGeometry((AbstractOptixGeometry*)data);
    }
        break;
    case OptixQListWidgetItem::Light: //Not yet implemented
        break;
    default:
        break;
    }
    m_sceneList->removeItemWidget(_item);
    delete _item;
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::addGeometry(AbstractOptixGeometry *_geo, QString _name)
{
    // Add the item to our inspector list
    m_sceneList->addItem(new OptixQListWidgetItem(_name,m_sceneList,OptixQListWidgetItem::Geometry,_geo));
    // Add the geometry to our renderer
    m_renderer->addGeometry(_geo);
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::removeGeometry(AbstractOptixGeometry *_geo)
{
    // Remove the geometry from our renderer
    m_renderer->removeGeometry(_geo);
    // Delete the geometry class
    delete _geo;
    _geo = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::clearScene()
{
    //select all items in list
    m_sceneList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_sceneList->selectAll();
    //delete all that are selected
    removeSelected();
    m_sceneList->setSelectionMode(QAbstractItemView::SingleSelection);
}
//----------------------------------------------------------------------------------------------------------------------
void InspectorMenu::renameSelected()
{
    bool ok;
    QList<QListWidgetItem*> items = m_sceneList->selectedItems();
    for(int i=0;i<items.size();i++)
    {
        QString tittle("Rename "+items[i]->text());
        QString name = QInputDialog::getText(this,tittle, tr("New name"),QLineEdit::Normal,items[i]->text(),&ok);
        if(ok && !name.isEmpty())
            items[i]->setText(name);
    }
}
//----------------------------------------------------------------------------------------------------------------------
