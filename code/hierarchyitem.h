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
    QPointF center;
    QList<EntityItem*> entities;
    QList<QPointF> points;
    bool isMoving, exclusive, total;

    QPointF *intersect(QList<QLineF> lines, QLineF line);
    QPointF *intersectRect(QRectF rect, QLineF line);
    QPointF rectangleCenter(EntityItem *item);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void load(QDomElement *e);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    enum { Type = UserType + 18 };

    HierarchyItem(GraphicView * graphicsView, Error * error, QWidget * parent,
                  QString typeName, bool exclusive, bool total, QList<EntityItem *> ents);


    bool getExclusive();
    bool getTotal();
    QString getTypeName();
    void setTypeName(QString typeName);

    QString getName();
    void setName(QString name);
    bool addAttribute(Attribute *attribute, QString parent);
    bool removeAttribute(QString attributesName);
    QList<Attribute*> getAllAttributes(); //parents and children
    QList<Attribute*> getAttributes(); //just parents
    void setPosition();
    void setPosition(QPointF pos);
    void setRelationship(Hierarchy *rship);
    QString getType();
    int type() const;
    void setGraphicView(GraphicView * view);
    GraphicView *getGraphicView();
    QList<EntityItem *> getEntities();
    QList<Cardinality *> getCardinalities();
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget *table);
    ERItem * getERItem();

    virtual void reloadEntities();
    virtual void removeFromEntities();
    virtual bool getIdDependency() { return false; }
    virtual IGraphicItem *getCopy();
    virtual void adjust();
    virtual QString getDerivation();
    virtual void getXML(QDomDocument *document, QDomElement *root);

};

#endif // HIERARCHYITEM_H
