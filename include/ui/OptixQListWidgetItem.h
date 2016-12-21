#ifndef OPTIXQLISTWIDGETITEM_H
#define OPTIXQLISTWIDGETITEM_H

#include <QListWidgetItem>
#include "common/AbstractOptixObject.h"

class OptixQListWidgetItem : public QListWidgetItem
{
public:
    enum {Geometry = ItemType::UserType, Light = ItemType::UserType+1};
    explicit OptixQListWidgetItem(QListWidget *view = 0, int type = Type, AbstractOptixObject* _optixObj = 0) : QListWidgetItem(view,type),m_optixObject(_optixObj){}
    explicit OptixQListWidgetItem(const QString &text, QListWidget *view = 0, int type = Type, AbstractOptixObject* _optixObj = 0): QListWidgetItem(text,view,type),m_optixObject(_optixObj){}
    explicit OptixQListWidgetItem(const QIcon &icon, const QString &text, QListWidget *view = 0, int type = Type, AbstractOptixObject* _optixObj = 0): QListWidgetItem(icon,text,view,type),m_optixObject(_optixObj){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor to our optix object
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixObject *getData(){return m_optixObject;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor
    //----------------------------------------------------------------------------------------------------------------------
    ~OptixQListWidgetItem(){}
    //----------------------------------------------------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our abstract optix object to store
    //----------------------------------------------------------------------------------------------------------------------
    AbstractOptixObject *m_optixObject;
    //----------------------------------------------------------------------------------------------------------------------
};

#endif // OPTIXQLISTWIDGETITEM_H
