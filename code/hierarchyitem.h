#ifndef HIERARCHYITEM_H
#define HIERARCHYITEM_H

#include "hierarchy.h"
#include "graphicview.h"
class EntityItem;

class HierarchyItem : public IGraphicItem
{
protected:
    Hierarchy *hierarchy;
    Error *error;
    QString name, typeName;
    QWidget *parent;
    GraphicView *graphicView;
    QList<EntityItem*> entities;


    QPointF rectangleCenter(EntityItem *item);
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

    void updateEntities();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    enum { Type = UserType + 18 };

    HierarchyItem(GraphicView * graphicsView,Error * error, QWidget * parent,QString typeName, bool exclusive,bool total, QList<EntityItem *> ents);
    QString getName();
    void setName(QString name);
    bool getExclusive();
    bool getTotal();
    QString getTypeName();
    void setTypeName(QString typeName);
    void removeEntity(EntityItem* item);
    QString getType();
    int type() const;
    void setGraphicView(GraphicView * view);
    GraphicView *getGraphicView();
    QList<EntityItem *> getEntities();
    ERItem * getERItem();
    virtual void reloadEntities();
    virtual void removeFromEntities();
    virtual IGraphicItem *getCopy();
    virtual QString getDerivation();
    virtual void getXML(QDomDocument *document,QDomElement *root);

    bool addAttribute(Attribute *attribute, QString parent);
    bool removeAttribute(QString attributesName);
    QList<Attribute*> getAllAttributes();
    QList<Attribute*> getAttributes();
    void setPosition();
    void setPosition(QPointF pos);
    void setRelationship(Hierarchy *rship);
    QList<Cardinality *> getCardinalities();
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget *table);

protected:
    void load(QDomElement *e);
};

#endif // HIERARCHYITEM_H
