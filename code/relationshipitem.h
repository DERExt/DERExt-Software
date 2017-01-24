#ifndef RELATIONSHIPITEM_H
#define RELATIONSHIPITEM_H
#include "relationship.h"
#include "graphicview.h"
#include <QPointF>
class EntityItem;

class RelationshipItem : public IGraphicItem
{
protected:
    Relationship * rship;
    Error *error;
    QWidget *parent;
    GraphicView *graphicView;
    QPointF center;
    QList<EntityItem*> entities;
    QList<QPointF> points;

    bool isMoving;
    int rhombusWidth;
    int rhombusHeigth;
    bool partOfAssociation;
    bool hasAssociationInSomeEntity;


    QPointF *intersect(QList<QLineF> lines, QLineF line);
    QPointF *intersectRect(QRectF rect, QLineF line);
    QPointF rectasngleCenter(EntityItem *item);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void load(QDomElement *e);

public:
    enum { Type = UserType + 11 };

    RelationshipItem(QString name, GraphicView * graphicsView, Error * error, QWidget * parent);

    QString getName();

    Relationship *getRship();

    void setName(QString name);
    bool addAttribute(Attribute *attribute, QString parent);
    bool removeAttribute(QString attributesName);
    QList<Attribute*> getAllAttributes(); //parents and children
    QList<Attribute*> getAttributes(); //just parents
    void setPosition();
    void setPosition(QPointF pos);
    void setRelationship(Relationship * rship);
    QString getType();
    int type() const;
    void setGraphicView(GraphicView * view);
    GraphicView *getGraphicView();
    QList<EntityItem *> getEntities();
    QList<Cardinality *> getCardinalities();
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget *table);
    ERItem * getERItem();
    QPointF rectangleCenter(EntityItem *item);

    void setPartOfAssociation();
    bool associationInSomeEntity();

    virtual void reloadEntities();
    virtual void removeFromEntities();
    virtual bool getIdDependency() { return false; }
    virtual IGraphicItem *getCopy()=0;
    virtual void adjust()=0;
    virtual QString getDerivation()=0;
    virtual void getXML(QDomDocument *document, QDomElement *root) = 0;

};

#endif // RELATIONSHIPITEM_H
