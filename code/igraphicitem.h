#ifndef IGRAPHICITEM_H
#define IGRAPHICITEM_H

#include "attribute.h"
#include "eritem.h"

#include <QGraphicsItem>
#include <QString>
#include <QtXml/QDomDocument>

class GraphicView;

class IGraphicItem : public QGraphicsItem
{
public:
    virtual QString getDerivation()=0;
    virtual QString getName()=0;
    virtual void setName(QString name)=0;
    virtual bool addAttribute(Attribute *attribute, QString parent)=0;
    virtual bool removeAttribute(QString attributesName)=0;
    virtual QList<Attribute*> getAllAttributes()=0; //parents and children
    virtual QList<Attribute*> getAttributes()=0; //just parents
    virtual void getXML(QDomDocument *document, QDomElement *root)=0;
    virtual void setPosition()=0;
    virtual void setPosition(QPointF pos)=0;
    virtual IGraphicItem *getCopy()=0;
    virtual QString getType()=0;
    virtual void setGraphicView(GraphicView * view) = 0;
    virtual GraphicView* getGraphicView() = 0;
    virtual bool moveAttribute(QString attribute, int row, int pos, QTableWidget * table)=0;
    virtual ERItem * getERItem()=0;
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value)=0;    //when removing from scene,
                                                                                        //some changes have to be done
};

#endif // IGRAPHICITEM_H
