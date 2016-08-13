#ifndef RTERNARYITEM_H
#define RTERNARYITEM_H

#include "relationshipitem.h"
#include "entityitem.h"
#include "rternary.h"


class RTernaryItem : public RelationshipItem
{
public:
    static const int expectedArity = 3;

    RTernaryItem(QString name, GraphicView * graphicsView, QDomElement * elem, Error * error, QWidget * parent,
                QList<EntityItem*> ents, QList<Cardinality*> cards);

    void adjust();

    IGraphicItem *getCopy();
    QString getDerivation();
    void getXML(QDomDocument *document, QDomElement *root);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // RTERNARYITEM_H
