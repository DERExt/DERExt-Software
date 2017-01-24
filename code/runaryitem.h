#ifndef RUNARYITEM_H
#define RUNARYITEM_H

#include "entityitem.h"
#include "rbinary.h"
#include <QWidget>

const int widthLoop = 75;
const int heightLoop = 75;

class RUnaryItem : public RelationshipItem
{
public:
    static const int expectedArity = 1;

    RUnaryItem(QString name, GraphicView * graphicsView, QDomElement * elem,Error * error, QWidget * parent,
                QList<EntityItem*> ents, QList<Cardinality*> cards);

    void adjust();

    IGraphicItem * getCopy();
    QString getDerivation();   
    void getXML(QDomDocument *document, QDomElement *root);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // RUNARYITEM_H
