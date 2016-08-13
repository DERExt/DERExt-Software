#ifndef RBINARYITEM_H
#define RBINARYITEM_H
#include <QtCore>
#include "graphicview.h"
#include "error.h"
#include "entityitem.h"
#include "relationshipitem.h"

class RBinaryItem : public RelationshipItem
{
public:
    static const int expectedArity = 2;

    RBinaryItem(QString name, GraphicView * graphicsView, QDomElement * elem, Error * error, QWidget * parent,
                QList<EntityItem *> ents, QList<Cardinality *> cards, bool idDependency = 0, bool firstWeak = 0);

    void adjust();

    IGraphicItem * getCopy();
    QString getDerivation();
    void getXML(QDomDocument *document, QDomElement *root);

    void reloadEntities();
    void removeFromEntities();
    bool getIdDependency();
    void setIdDependency(bool idDep, bool firstWeak = 0);

protected:
    bool idDependency;
    bool firstWeak;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // RBINARYITEM_H
