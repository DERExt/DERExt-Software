#ifndef EntityItem_H
#define EntityItem_H

#include "attribute.h"
#include "entity.h"
#include "igraphicitem.h"
#include "error.h"
#include "attdialogedit.h"
#include "attsimple.h"
#include "attcomp.h"
#include "graphicview.h"
#include "relationshipitem.h"
#include "hierarchyitem.h"

#include <QGraphicsItem>
#include <QList>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTableWidget>

#include "qdialog.h"
#include "QtCore"
#include "QtGui"

class EntityItem : public IGraphicItem
{
private:
    QList<RelationshipItem*> rships;
    int width, height;
    GraphicView *graphicView;
    Entity *ent;
    Error *error;
    QWidget *parent;
    QPointF position;
    bool isWeak, isMoving;
    bool subtype, old;
    QString typeElement;

    HierarchyItem *fatherH = NULL;
    HierarchyItem *sonH = NULL;
    bool newInDERE;

private:
    void loadNode(QDomElement *element);

    void setBounds(QString entityName);
    void printElements();

    QPointF * getMostBottomComponent();
    QPointF * getMostTopComponent();
    QPointF * getMostRightComponent();
    QPointF * getMostLeftComponent();



public:
    enum { Type = UserType + 10};
    static const int charWidth = 5;
    static const int charHeight = 14;


    EntityItem(QString entityName, QDomElement *element, GraphicView *graphicView, Error *error, QWidget *parent,
               bool sub, bool old, bool weak = 0, Entity *strongEnt = 0, bool isAssociation = false, RelationshipItem * rel = 0);
    bool addAttribute(Attribute *attribute, QString parent);
    bool removeAttribute(QString attributesName);
    QString getDerivation();
    QString getName();
    void setName(QString name);
    QList<Attribute*> getAllAttributes();
    QList<Attribute*> getAttributes();
    void getXML(QDomDocument *document, QDomElement *root);
    void setPosition();
    void setPosition(QPointF pos);
    IGraphicItem *getCopy();
    void setEntity(Entity *ent);
    QString getType();
    int type() const;
    QPointF getPosition();

    bool isSubtype();
    void setSubtype(bool op);

    void setGraphicView(GraphicView * view);
    GraphicView *getGraphicView();
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget * table);
    ERItem *getERItem();

    void setIsWeak(bool weak, EntityItem *strongEnt);
    bool getIsWeak();
    bool getHasIdDependency();

    void addRelationship(RelationshipItem * item);

    void removeRelationship(RelationshipItem * item);
    int getWidth();
    int getHeight();

    bool isFather() const;
    void setHierarchy(HierarchyItem *item);
    HierarchyItem* getHierarchy();
    void setHierarchySon(HierarchyItem *item);
    HierarchyItem *getHierarchySon();
    void RemoveHierarchySon();
    void RemoveHierarchy();
    void setOldState(bool state);
    int x() { return this->position.x(); };
    int y() { return this->position.y();};

    void setAssociation(RelationshipItem * rel);
    QRectF getRectangle();


    bool isAssociation;
    QPointF centerRect;
    RelationshipItem * aggregationRel;
    QPointF getRectangleCenter();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // EntityItem_H
