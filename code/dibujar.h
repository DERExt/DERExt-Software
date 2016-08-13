#ifndef DIBUJAR_H
#define DIBUJAR_H

#include "hierarchyitem.h"
#include "entityitem.h"

class Dibujar : public HierarchyItem
{
public:
    Dibujar(QString name, GraphicView * graphicsView, QDomElement * elem, Error * error, QWidget * parent,
            QList<EntityItem*> ents, QList<Cardinality*> cards);

    IGraphicItem *getCopy();
    QString getDerivation();
    void getXML(QDomDocument *document, QDomElement *root);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // DIBUJAR_H
