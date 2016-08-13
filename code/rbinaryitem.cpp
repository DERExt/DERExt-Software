#include "rbinaryitem.h"
#include "rbinary.h"
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

RBinaryItem::RBinaryItem(QString name, GraphicView *graphicsView, QDomElement *elem, Error *error, QWidget *parent,
                         QList<EntityItem*> ents, QList<Cardinality*> cards, bool idDependency, bool firstWeak):
    RelationshipItem(name,graphicsView,error,parent)
{
    if (ents.size() == expectedArity && cards.size() == expectedArity){
        entities.append(ents);
        entities.at(0)->addRelationship(this);
        entities.at(1)->addRelationship(this);
        adjust();
        this->idDependency = idDependency;
        this->firstWeak = firstWeak;

        QList<Entity*> entityList;
        entityList << (Entity*)entities.at(0)->getERItem() << (Entity*)entities.at(1)->getERItem();
        this->rship = new RBinary(name,error,entityList,cards);
        load(elem);

        this->setFlag(ItemIsMovable);
    }
}

IGraphicItem *RBinaryItem::getCopy()
{
    QList<Cardinality*> card;
    foreach(Cardinality * c, rship->getCardinalities()){
        Cardinality * newCard = new Cardinality(c->getMin(), c->getMax());
        card.append(newCard);
    }
    RBinaryItem * item;
    if (card.size() == expectedArity && entities.size() == expectedArity){
        QList<EntityItem*> entitiesCopy;
        entitiesCopy << (EntityItem*)entities.at(0)->getCopy() << (EntityItem*)entities.at(1)->getCopy();
        item = new RBinaryItem(rship->getName(),graphicView,NULL,error,parent, entitiesCopy, card, idDependency, firstWeak);
        item->setRelationship(rship->getCopy());
        item->setPosition(center);
    }
    return item;
}

QString RBinaryItem::getDerivation()
{
    return rship->getDerivation();
}

void RBinaryItem::getXML(QDomDocument *document, QDomElement *root)
{
    QDomElement * tmp = ((RBinary*)rship)->getXML(document);
    tmp->setAttribute("x",center.x());
    tmp->setAttribute("y",center.y());

    tmp->setAttribute("dep",this->idDependency);
    tmp->setAttribute("firstWeak",this->firstWeak);

    root->appendChild(*tmp);
}

void RBinaryItem::reloadEntities()
{
    RelationshipItem::reloadEntities();
    if (this->idDependency){
        if (firstWeak)
            entities.at(0)->setIsWeak(true,entities.at(1));
        else entities.at(1)->setIsWeak(true,entities.at(0));

        //Because setIsWeak makes a new entity, such has to be updated in rship.
        //RelationshipItem has to do the same for others RshipItems, but with this one we need
        //to do it after the setIsWeak.
        QList<Entity*> ents;
        foreach(EntityItem * ent, entities)
            ents << (Entity*)ent->getERItem();
        rship->setEntities(ents);
    }
}

void RBinaryItem::removeFromEntities()
{
    if (this->idDependency){
        if (firstWeak)
            entities.at(0)->setIsWeak(false,NULL);
        else entities.at(1)->setIsWeak(false,NULL);
    }
    RelationshipItem::removeFromEntities();
}

bool RBinaryItem::getIdDependency()
{
    return this->idDependency;
}

void RBinaryItem::setIdDependency(bool idDep, bool firstWeak)
{
    if (entities.size()!=expectedArity) return;

    this->idDependency = idDep;
    if (idDependency){
        if (firstWeak) entities.at(0)->setIsWeak(true,entities.at(1));
        else entities.at(1)->setIsWeak(true,entities.at(0));
        this->firstWeak = firstWeak;

        //Because setIsWeak makes a new Entity, we must update such Entity in our Relationship.
        QList<Entity*> ents;
        ents << (Entity*)entities.at(0)->getERItem() << (Entity*)entities.at(1)->getERItem();
        rship->setEntities(ents);
    }
    else{
        if (this->firstWeak)
            entities.at(0)->setIsWeak(false,NULL);
        else entities.at(1)->setIsWeak(false,NULL);
    }
}


QRectF RBinaryItem::boundingRect() const{
    if (entities.size() != expectedArity || points.size() != expectedArity)
        return QRectF();

    int minX = MIN(center.x(),MIN(points.at(0).x(),points.at(1).x()));
    int minY = MIN(center.y(),MIN(points.at(0).y(),points.at(1).y()));
    int maxX = MAX(center.x(),MAX(points.at(0).x(),points.at(1).x()));
    int maxY = MAX(center.y(),MAX(points.at(0).y(),points.at(1).y()));

    int longestAttribute = 0;
    foreach(Attribute * att, rship->getAllAttributes())
        if (att->getName().count() > longestAttribute)
            longestAttribute = att->getName().count();

    return QRectF(QPoint(minX,minY),QPoint(maxX,maxY))
           .normalized()
           .adjusted(-rhombusWidth, -rhombusHeigth,
                     rhombusWidth+longestAttribute*EntityItem::charWidth+40,
                     rhombusHeigth+rship->getAllAttributes().size()*EntityItem::charHeight);
}

void RBinaryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
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
    QLineF line(points.at(0),center);
    painter->drawLine(line);
    QLineF line2(points.at(1),center);
    painter->drawLine(line2);
    if (entities.at(0)->getIsWeak() && idDependency && firstWeak)
        painter->drawLine(QLineF(points.at(0)+QPointF(3,3),center+QPointF(3,3)));
    if (entities.at(1)->getIsWeak() && idDependency && !firstWeak)
        painter->drawLine(QLineF(points.at(1)+QPointF(3,3),center+QPointF(3,3)));

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
    if (cards.size() == expectedArity && points.size() == entities.size()){         //tamaño correcto que deberia tener en relacion binaria
        QPointF center(centerX,centerY);

        QList<QLineF> lines;
        lines << line << line2;
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

void RBinaryItem::adjust(){
    if (entities.size() != expectedArity)
        return;

    prepareGeometryChange();

    if (points.size() == expectedArity)
        points.clear();
    points << rectangleCenter(entities.at(0)) << rectangleCenter(entities.at(1));
    center = (points.at(0)+points.at(1))/2;
}


