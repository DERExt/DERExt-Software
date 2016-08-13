#include "rternaryitem.h"
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

RTernaryItem::RTernaryItem(QString name, GraphicView *graphicsView, QDomElement *elem, Error *error, QWidget *parent,
                           QList<EntityItem *> ents, QList<Cardinality *> cards):
    RelationshipItem(name,graphicsView,error,parent)
{
    if (ents.size() == expectedArity && cards.size() == expectedArity){
        this->entities << ents;
        ents.at(0)->addRelationship(this);
        ents.at(1)->addRelationship(this);
        ents.at(2)->addRelationship(this);
        adjust();

        QList<Entity*> entityList;
        entityList << (Entity*)ents.at(0)->getERItem() << (Entity*)ents.at(1)->getERItem() << (Entity*)ents.at(2)->getERItem();
        this->rship = new RTernary(name,error,entityList,cards);
        load(elem);
        this->setFlag(ItemIsMovable);
    }
}

void RTernaryItem::adjust()
{
    if (entities.size() != expectedArity)
        return;

    QLineF line(rectangleCenter(entities.at(0)), rectangleCenter(entities.at(1)));

    prepareGeometryChange();

    if (points.size() == expectedArity) points.clear();
    points << line.p1() << line.p2() << rectangleCenter(entities.at(2));
    if (entities.at(0)!=entities.at(1))
        center = (line.p1()+line.p2())/2;
    else center = (line.p1()+points.at(2))/2;
}

IGraphicItem *RTernaryItem::getCopy()
{
    QList<Cardinality*> card;
    foreach(Cardinality * c, rship->getCardinalities()){
        Cardinality * newCard = new Cardinality(c->getMin(), c->getMax());
        card.append(newCard);
    }
    RTernaryItem * item;
    if (card.size() == expectedArity && entities.size() == expectedArity){
        QList<EntityItem*> entitiesCopy;
        entitiesCopy << (EntityItem*)entities.at(0)->getCopy() << (EntityItem*)entities.at(1)->getCopy()
                    << (EntityItem*)entities.at(2)->getCopy();
        item = new RTernaryItem(rship->getName(),graphicView,NULL,error,parent, entitiesCopy, card);
        item->setRelationship(rship->getCopy());
        item->setPosition(center);
    }
    return item;
}

QString RTernaryItem::getDerivation()
{
    return this->rship->getDerivation();
}

void RTernaryItem::getXML(QDomDocument *document, QDomElement *root)
{
    QDomElement * tmp = ((RTernary*)rship)->getXML(document);
    tmp->setAttribute("x",center.x());
    tmp->setAttribute("y",center.y());
    root->appendChild(*tmp);
}

QRectF RTernaryItem::boundingRect() const
{
    if (entities.size() != expectedArity || points.size() != expectedArity)
        return QRectF();

    int longestAtt = 0;
    foreach(Attribute * att, rship->getAllAttributes())
        if (att->getName().count() > longestAtt)
            longestAtt = att->getName().count();

    int minX = MIN(points.at(0).x(),MIN(points.at(1).x(),MIN(points.at(2).x(),center.x())));                   //minimo de X
    int maxX = MAX(points.at(0).x(),MAX(points.at(1).x(),MAX(points.at(2).x(),center.x())));                   //max de X
    int minY = MIN(points.at(0).y(),MIN(points.at(1).y(),MIN(points.at(2).y(),center.y())));                   //minimo de Y
    int maxY = MAX(points.at(0).y(),MAX(points.at(1).y(),MAX(points.at(2).y(),center.y())));                   //max de Y

    return QRectF(QPointF(minX-rhombusWidth,minY-rhombusHeigth),
                  QPointF(maxX+rhombusWidth+longestAtt*EntityItem::charWidth+40
                          ,maxY+rhombusHeigth+rship->getAttributes().size()*EntityItem::charHeight+20));
            //se agregan los offsets para considerar rombo y atributos
}

void RTernaryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (entities.size() != expectedArity || points.size() != expectedArity)
        return;

    double centerX = center.x();
    double centerY = center.y();
    QPointF center(centerX,centerY);
    QPointF x1(centerX - rhombusWidth , centerY);
    QPointF x2(centerX + rhombusWidth , centerY);
    QPointF y1(centerX , centerY - rhombusHeigth);
    QPointF y2(centerX , centerY + rhombusHeigth);

    //conexion
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QLineF line(points.at(0),center);
    QLineF line2(points.at(1),center);
    painter->drawLine(line);
    painter->drawLine(line2);
    painter->drawLine(center,points.at(2));

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
    QList<Cardinality*> cards = rship->getCardinalities();
    QList<QLineF> rhombusLines;
    rhombusLines << QLineF(x1,y1) << QLineF(y1,x2) << QLineF(x2,y2) << QLineF(y2,x1);

    if (cards.size() == expectedArity){         //tamaño correcto que deberia tener en relacion binaria
        QPointF center(centerX,centerY);
        QList<QLineF> lines;
        lines << line << line2 << QLineF(center,points.at(2));
        for (int i = 0; i < points.size(); i++){
            QPointF * intersEnt = intersectRect(QRectF(points.at(i)-QPointF(entities.at(i)->getWidth()/2,entities.at(i)->getHeight()/2),
                                                points.at(i)+QPointF(entities.at(i)->getWidth()/2,entities.at(i)->getHeight()/2)),lines.at(i));
            QPointF * intersRhomb;
            if (intersEnt != NULL)
                intersRhomb = intersect(rhombusLines,QLineF(center,*intersEnt));
            if (intersEnt != NULL && intersRhomb != NULL)
                painter->drawText((*intersEnt+*intersRhomb)/2 + QPointF(4,-4),cards.at(i)->getText());
        }
    }
}
