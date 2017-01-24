#include "relationshipitem.h"
#include "entity.h"
#include "entityitem.h"
/**
 * Constructor distinto a EntityItem ya que todavia no se
 * realiza guardado de relaciones.
 */
RelationshipItem::RelationshipItem(QString name, GraphicView * graphicsView, Error * error, QWidget * parent)
{
    this->graphicView = graphicsView;
    this->error = error;
    this->parent = parent;
    rhombusWidth = (name.count()*EntityItem::charWidth <= 40) ? 40 : name.count()*EntityItem::charWidth + 10;
    isMoving = false;
    rhombusHeigth = 25;
    this->partOfAssociation = false;
    this->hasAssociationInSomeEntity = false;
}

Relationship * RelationshipItem::getRship() {
    return this->rship;
}


QString RelationshipItem::getName()
{
    return this->rship->getName();
}

void RelationshipItem::setName(QString name)
{
    this->rship->setName(name);
    rhombusWidth = (name.count()*EntityItem::charWidth < 40) ? 40 : name.count()*EntityItem::charWidth + 10;
}

bool RelationshipItem::addAttribute(Attribute *attribute, QString parent)
{
    if (graphicView)
        graphicView->setSaved(false);
    return rship->addAttribute(attribute,parent,error);
}

bool RelationshipItem::associationInSomeEntity() {
    return this->hasAssociationInSomeEntity;
}

void RelationshipItem::setPartOfAssociation() {
    this->partOfAssociation = true;
}

bool RelationshipItem::removeAttribute(QString attributesName)
{
    if (graphicView)
        graphicView->setSaved(false);
    return rship->removeAttribute(attributesName);
}

QList<Attribute *> RelationshipItem::getAllAttributes()
{
    return rship->getAllAttributes();
}

QList<Attribute *> RelationshipItem::getAttributes()
{
    return rship->getAttributes();
}

void RelationshipItem::setPosition()
{
    //this->center = this->pos();
}

void RelationshipItem::setPosition(QPointF pos)
{
    this->center = pos;
}

void RelationshipItem::setRelationship(Relationship *rship)
{
    this->rship = rship;
}

QString RelationshipItem::getType()
{
    return QString("Relationship");
}

void RelationshipItem::setGraphicView(GraphicView *view)
{
    this->graphicView = view;
}

GraphicView *RelationshipItem::getGraphicView()
{
    return this->graphicView;
}

int RelationshipItem::type() const
{
    return Type;
}

QList<EntityItem *> RelationshipItem::getEntities()
{
    return this->entities;
}

QList<Cardinality *> RelationshipItem::getCardinalities()
{
    return this->rship->getCardinalities();
}

bool RelationshipItem::moveAttribute(QString attribute, int row, int newPos, QTableWidget *table)
{
    return this->rship->moveAttribute(attribute,row,newPos,table);
}

ERItem *RelationshipItem::getERItem()
{
    return this->rship;
}

void RelationshipItem::removeFromEntities()
{
    foreach(EntityItem* item, entities){
        item->removeRelationship(this);
    }
}

void RelationshipItem::reloadEntities()
{
    //when changing scene, the pointer to the entities must be updated to the ones in such scene.
    if (graphicView != NULL){
        QList<EntityItem*> result;
        foreach(EntityItem* item , entities)
            result.append((EntityItem*)this->graphicView->getItem(item->getName()));
        entities.clear();
        entities.append(result);
        removeFromEntities();
        foreach(EntityItem* item , entities)
            item->addRelationship(this);

        QList<Entity*> ents;
        foreach(EntityItem * ent, entities)
            ents << (Entity*)ent->getERItem();
        if (entities.size() == 1)       //unary rship
            ents << (Entity*)entities.at(0)->getERItem();
        rship->setEntities(ents);
    }
}

QPointF RelationshipItem::rectangleCenter(EntityItem *item) {
    qreal centerX;
    qreal centerY;

    if(item->isAssociation) {
        this->hasAssociationInSomeEntity = true;
        return item->getRectangleCenter();
    }
    else {
        centerX = (item->pos().x() + item->getWidth() /2);
        centerY = (item->pos().y() + item->getHeight()/2);
        return QPointF(centerX,centerY);
    }
}

QPointF *RelationshipItem::intersect(QList<QLineF> lines, QLineF line)
{
    QPointF * result = new QPointF();
    foreach(QLineF l, lines){
        if (line.intersect(l,result) == QLineF::BoundedIntersection)
            return result;
    }
    return NULL;
}

QPointF * RelationshipItem::intersectRect(QRectF rect, QLineF line){
    QList<QLineF> lines;
    lines << QLineF(rect.topLeft(),rect.topRight())
        << QLineF(rect.bottomLeft(),rect.bottomRight())
        << QLineF(rect.topLeft(),rect.bottomLeft())
        << QLineF(rect.topRight(),rect.bottomRight());

    return intersect(lines,line);
}

QVariant RelationshipItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneChange){
        if (this->scene() == NULL)          //agregado por primera vez
            reloadEntities();
        else
            removeFromEntities();
    }
    return QGraphicsItem::itemChange(change,value);
}

void RelationshipItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF rhombusRect(center.x()-rhombusWidth,center.y()-rhombusHeigth,
                       rhombusWidth*2,rhombusHeigth*2);
    if (rhombusRect.contains(event->pos())){
        isMoving = true;
        QGraphicsItem::mousePressEvent(event);
    }
    else event->ignore();
}

void RelationshipItem::mouseMoveEvent(QGraphicsSceneMouseEvent*event)
{
    //it's necessary that mouseMoveEvent from QGraphicsItem is not called
    //in order to allow releaseEvent to actually adjust the rhombus.
    //QGraphicsItem::mouseMoveEvent(event);
}

void RelationshipItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isMoving){
        adjust();
        center = event->pos();
    }
    isMoving = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void RelationshipItem::load(QDomElement *e)
{
    if(e != NULL){
            QDomNodeList nodes = e->childNodes();
            for(int i=0 ; i<nodes.count() ; i++){
                QDomNode node = nodes.at(i);
                if(node.isElement()){
                    QDomElement element = node.toElement();
                    Attribute *newAtt;
                    QString prnt = element.attribute("parent");
                    bool bprnt = !prnt.isEmpty();
                    if(element.tagName()=="Simple"){
                        newAtt = new AttSimple(element.attribute("name"), element.attribute("null").toInt(), element.attribute("mult").toInt(), element.attribute("pk").toInt(), element.attribute("sk").toInt(), bprnt);
                        ((AttSimple*)newAtt)->setType(element.attribute("type"), element.attribute("cant1").toInt(), element.attribute("cant2").toInt(), element.attribute("cant3").toInt());
                    }
                    else if (element.tagName()=="Compound")
                        newAtt = new AttComp(element.attribute("name"), element.attribute("null").toInt(), element.attribute("amount").toInt(), element.attribute("mult").toInt(), element.attribute("pk").toInt(), element.attribute("sk").toInt(), bprnt);
                    newAtt->setOptionSQL(element.attribute("optionSQL").toInt());
                    this->addAttribute(newAtt, prnt);
                    this->load(&element);
                }
            }
    }
}



