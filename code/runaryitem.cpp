#include "runaryitem.h"
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

RUnaryItem::RUnaryItem(QString name, GraphicView * graphicsView, QDomElement *elem, Error * error, QWidget * parent,
                       QList<EntityItem *> ents, QList<Cardinality *> cards):
    RelationshipItem(name,graphicsView,error,parent)
{
    if (ents.size() == expectedArity && cards.size() == expectedArity+1){
        entities << ents;
        ents.at(0)->addRelationship(this);

        QList<Entity*> entityList;
        entityList << (Entity*)ents.at(0)->getERItem() << (Entity*)ents.at(0)->getERItem();
        rship = new RBinary(name,error,entityList,cards);
        load(elem);
        adjust();
        this->setFlag(ItemIsMovable,true);
    }
}


void RUnaryItem::adjust()
{
    if (entities.size() != expectedArity) return;

    prepareGeometryChange();

    EntityItem * entity = entities.at(0);
    if (points.size() == expectedArity) points.clear();
    points << QPointF(entity->x()+entity->getWidth()-10,entity->y()+15);
    center = QPointF(points.at(0).x()+widthLoop-1,points.at(0).y()-heightLoop+10);
}

IGraphicItem *RUnaryItem::getCopy()
{
    QList<Cardinality*> card;
    foreach(Cardinality * c, rship->getCardinalities()){
        Cardinality * newCard = new Cardinality(c->getMin(), c->getMax());
        card.append(newCard);
    }

    RUnaryItem * item;
    if (card.size() == expectedArity + 1 && entities.size()== expectedArity){
        QList<EntityItem*> entitiesCopy;
        entitiesCopy << (EntityItem*)entities.at(0)->getCopy();
        item = new RUnaryItem(rship->getName(),graphicView,NULL,error,parent,entitiesCopy,card);
        item->setRelationship(rship->getCopy());
        item->setPosition(center);
    }
    return item;
}

QString RUnaryItem::getDerivation()
{
    return rship->getDerivation();
}

void RUnaryItem::getXML(QDomDocument *document, QDomElement *root)
{
    QDomElement * tmp = (rship->getXML(document,"RUnary"));
    tmp->setAttribute("x",center.x());
    tmp->setAttribute("y",center.y());
    root->appendChild(*tmp);
}

QRectF RUnaryItem::boundingRect() const
{
    if (entities.size() != expectedArity || points.size() != expectedArity) return QRectF();

    int minX = MIN(points.at(0).x(),center.x());
    int maxX = MAX(points.at(0).x(),center.x());
    int minY = MIN(points.at(0).y(),center.y());
    int maxY = MAX(points.at(0).y(),center.y());

    int longestAttribute = 0;
    foreach(Attribute * att, rship->getAllAttributes())
        if (att->getName().count() > longestAttribute)
            longestAttribute = att->getName().count();

    return QRectF(QPointF(minX-rhombusWidth,minY-rhombusHeigth*2),
                  QPointF(maxX+longestAttribute*EntityItem::charWidth+rhombusWidth+40
                          ,maxY+rhombusHeigth*2+rship->getAllAttributes().size()*EntityItem::charHeight));
}

void RUnaryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (entities.size() != expectedArity || points.size() != expectedArity)
        return;

    double centerX = center.x();
    double centerY = center.y();
    QPointF x1(centerX - rhombusWidth , centerY);
    QPointF x2(centerX + rhombusWidth , centerY);
    QPointF y1(centerX , centerY - rhombusHeigth);
    QPointF y2(centerX , centerY + rhombusHeigth);

    //conexion
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    /**
     * | LADO IZQUIERDO
     */

    QLineF line1(
        QPointF(points.at(0).x(), center.y()),
        QPointF(points.at(0).x(),points.at(0).y())
    );

    /**
     * _ ABAJO
     */

    QLineF line2(
        QPointF(center.x(),points.at(0).y()),
        QPointF(points.at(0).x(),points.at(0).y())
    );

    /**
     * | DERECHO
     */

    QLineF line3(
        QPointF(center.x(),center.y()),
        QPointF(center.x(),points.at(0).y())
    );

    /**
     * _ ARRIBA
     */

    QLineF line4(
        QPointF(center.x(),center.y()),
        QPointF(points.at(0).x(), center.y())
    );

    foreach(EntityItem * item, this->getEntities()) {
        QPointF * in = intersectRect(item->getRectangle(), line1);
        if (in != NULL) {
            QPointF pInt(*in);
            QLineF newLine(pInt,QPointF(points.at(0).x(), center.y()));
            line1 = newLine;
        }
    }

    foreach(EntityItem * item, this->getEntities()) {
        QPointF * in = intersectRect(item->getRectangle(), line2);
        if (in != NULL) {
            QPointF pInt(*in);
            QLineF newLine(pInt,QPointF(center.x(),points.at(0).y()));
            line2 = newLine;
        }
    }
    foreach(EntityItem * item, this->getEntities()) {
        QPointF * in = intersectRect(item->getRectangle(), line3);
        if (in != NULL) {
            QPointF pInt(*in);
            QLineF newLine(pInt, points.at(0));
            line3 = newLine;
        }
    }

    foreach(EntityItem * item, this->getEntities()) {
        QPointF * in = intersectRect(item->getRectangle(), line4);
        if (in != NULL) {
            QPointF pInt(*in);
            QLineF newLine(pInt, points.at(0));
            line4 = newLine;
        }
    }

    painter->drawLine(line1);
    painter->drawLine(line2);
    painter->drawLine(line3);
    painter->drawLine(line4);

    //atributos
    int x = centerX + 30;
    int y = centerY + 10;
    foreach(Attribute * att, rship->getAttributes()){
        QList<Attribute*> lastAtts = att->getChildren();
        if(att->compare(rship->getAttributes().last()) && !lastAtts.isEmpty()){
            att->addToScene(painter, x, y);
            y+=15; //last is compound, so the vertical line shouldn't go to the children
        }
        else y = att->addToScene(painter, x, y);
    }
    if (!rship->getAttributes().isEmpty())
        painter->drawLine(QPoint(x,centerY),QPoint(x,y-3));

    //rombo
    painter->setBrush(Qt::white);
    painter->drawPolygon(QPolygonF() << x1 << y1 << x2 << y2);

    painter->drawText(QRectF(QPointF(centerX-rhombusWidth/2,centerY-rhombusHeigth/2),
                             QPointF(centerX+rhombusWidth/2,centerY+rhombusHeigth/2))
                      , Qt::AlignCenter, rship->getName());

    //cardinalidades
    QList<Cardinality *> cards = rship->getCardinalities();
    if (cards.size() == expectedArity +1){         //tama?o correcto que deberia tener en relacion unaria
        QPointF middle = (points.at(0)+center)/2;
        painter->drawText(middle + QPointF(0,heightLoop-50),cards.at(0)->getText());
        painter->drawText(middle - QPointF(10,heightLoop-25),cards.at(1)->getText());
    }
}

