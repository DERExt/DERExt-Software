#include "entityitem.h"
#include "weakentity.h"

#include <QRectF>
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>


#include <QDebug>

EntityItem::EntityItem(QString entityName, QDomElement *element, GraphicView *graphicView, Error *error, QWidget *parent,
                       bool weak, Entity * strongEnt)
{
    this->width = entityName.count()*charWidth + 20;
    this->height = 40;
    this->graphicView = graphicView;
    this->error = error;
    this->parent = parent;
    this->isWeak = weak;
    this->isMoving = false;
    if (isWeak)
        this->ent = new WeakEntity(entityName, this->error, strongEnt);
    else
        this->ent = new Entity(entityName, this->error);
    this->setFlag(ItemIsMovable, true);
    this->setFlag(ItemSendsGeometryChanges,true);
    this->setFlag(ItemIsSelectable,true);
    this->loadNode(element);
    this->ent->generateCompSK();
    if(graphicView)
        this->graphicView->setSaved(true);
    this->setPosition();

}

void EntityItem::loadNode(QDomElement *e)
{
    if(e!=NULL){
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
                if(element.attribute("pk").toInt() == true)
                    this->ent->setPrimaryKey(newAtt);
                if(element.attribute("sk").toInt() == true)
                    this->ent->setSecondaryKey(newAtt);
                this->loadNode(&element);
            }
        }
    }
}


bool EntityItem::addAttribute(Attribute *attribute, QString parent)
{
    if(graphicView)
        graphicView->setSaved(false);
    return ent->addAttribute(attribute, parent, error);
}

bool EntityItem::removeAttribute(QString attributesName)
{
    if(graphicView)
        graphicView->setSaved(false);
    return ent->removeAttribute(attributesName);
}

QString EntityItem::getDerivation()
{
    return this->ent->getDerivation();
}

QString EntityItem::getName()
{
    return this->ent->getName();
}

void EntityItem::setName(QString name)
{
    this->ent->setName(name);
    this->width = name.count()*charWidth + 20;
}

QList<Attribute *> EntityItem::getAllAttributes()
{
    return this->ent->getAllAttributes();
}

QList<Attribute *> EntityItem::getAttributes()
{
    return this->ent->getAttributes();
}

void EntityItem::getXML(QDomDocument *document, QDomElement *root)
{
    QDomElement *tmp = ent->getXML(document);

    tmp->setAttribute("x", this->pos().x());
    tmp->setAttribute("y", this->pos().y());

    QList<QString> aux = ent->getSKToSave();
    foreach(QString s, aux){
        QDomElement element = document->createElement("compSK");
        element.setAttribute("attName", s);
        tmp->appendChild(element);
    }

    root->appendChild(*tmp);
}

void EntityItem::setPosition()
{
    this->position = this->pos();
}

void EntityItem::setPosition(QPointF pos)
{
    this->position = pos;
    this->setPos(pos);
}

IGraphicItem *EntityItem::getCopy()
{
    EntityItem * tmp;
    if (isWeak)
        tmp = new EntityItem(ent->getName(), NULL, this->graphicView, this->error, this->parent, isWeak,
                                      ((WeakEntity*)ent)->getStrongEntity());
    else tmp = new EntityItem(ent->getName(), NULL, this->graphicView, this->error, this->parent);
    tmp->setPosition(this->pos());
    tmp->setEntity(ent->getCopy());
    return tmp;
}

void EntityItem::setEntity(Entity *ent)
{
    this->ent = ent;
}

QString EntityItem::getType()
{
    return QString("Entity");
}

int EntityItem::type() const
{
    return Type;
}

QPointF EntityItem::getPosition()
{
    return this->position;
}

void EntityItem::setFather(EntityItem *item)
{
    this->ent->setFather((Entity*)item->getERItem());
}

Entity *EntityItem::getFather()
{
    return this->ent->getFather();
}

void EntityItem::deleteFather(){
    this->ent->deleteFather();
}

void EntityItem::setGraphicView(GraphicView *view)
{
    this->graphicView = view;
}

GraphicView *EntityItem::getGraphicView()
{
    return this->graphicView;
}

bool EntityItem::moveAttribute(QString attribute, int row, int pos, QTableWidget *table)
{
    return this->ent->moveAttribute(attribute,row,pos,table);
}

ERItem *EntityItem::getERItem()
{
    return this->ent;
}

void EntityItem::setIsWeak(bool weak, EntityItem * strongEnt)
{
    this->isWeak = weak;
    QList<Attribute*> atts = this->ent->getAttributes();
    if (weak && strongEnt != NULL)
        this->ent = new WeakEntity(this->getName(), this->error, (Entity*)strongEnt->getERItem());
    else this->ent = new Entity(this->getName(),this->error);
    foreach (Attribute * att, atts)
        ent->addAttribute(att,"",error);
}

void EntityItem::addRelationship(RelationshipItem *item)
{
    rships.append(item);
}

void EntityItem::addHierarchy(HierarchyItem *item)
{
    hrchy.append(item);
}

void EntityItem::removeRelationship(RelationshipItem *item)
{
    rships.removeAll(item);
}

void EntityItem::removeHierarchy(HierarchyItem *item)
{
    hrchy.removeAll(item);
}

int EntityItem::getWidth()
{
    return width;
}

int EntityItem::getHeight(){
    return height;
}


bool EntityItem::getIsWeak()
{
    return isWeak;
}

bool EntityItem::getHasIdDependency()
{
    foreach(RelationshipItem * r,rships)
        if (r->getIdDependency())
            return true;
    return false;
}

QRectF EntityItem::boundingRect() const
{
    int longestAtt = 0;
    foreach(Attribute * att, this->ent->getAllAttributes()){
        if (att->getName().count()*charWidth > longestAtt)
            longestAtt = att->getName().count()*charWidth;
    }
    return QRectF(-2,-2,width+longestAtt*charWidth+5, height+this->ent->getAllAttributes().size()*charHeight +2);
}

void EntityItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF br = boundingRect();
    br.setWidth(br.width()-30);
    QPen pen = painter->pen();
    painter->setBrush(Qt::white);
    if (this->isSelected()) painter->setPen(Qt::DashLine);
    painter->drawRect(QRectF(QPointF(0,0),QPointF(width,height)));
    if (isWeak)
        painter->drawRect(QRectF(QPointF(3,3),QPointF(width-3,height-3)));
    painter->drawText(QRectF(QPointF(0,0),QPointF(width,height)), Qt::AlignCenter, ent->getName());

    QList<Attribute*> atts = ent->getAttributes();
    //Attribute *last;

    if(!atts.isEmpty()){
        double y = 15, x = width+10;
        foreach(Attribute *att, atts){
            QList<Attribute*> lastAtts = att->getChildren();
            if(att->compare(atts.last()) && !lastAtts.isEmpty()){
                att->addToScene(painter, x, y);
                y+=15; //last is compound, so the vertical line shouldn't go to the children
            }
            else y = att->addToScene(painter, x, y);
        }

        painter->setPen(pen);
        painter->drawLine(QPoint(width, 27), QPoint(width+10, 27)); //first small horizontal line
        painter->drawLine(QPoint(width+10, 27), QPoint(width+10, y-3)); //long vertical line
    }

    if(this->position!=this->pos())
        this->graphicView->setSaved(false);
}
QVariant EntityItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if  (change == ItemSceneHasChanged && this->scene()==NULL){
        foreach(RelationshipItem * rship, rships){
            rship->removeFromEntities();
            if (rship->scene()!=NULL) rship->scene()->removeItem(rship);
        }
    }
    if (change == ItemPositionHasChanged){
        foreach(RelationshipItem * rship, rships){
            rship->adjust();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
/**
 * @brief EntityItem::mousePressEvent
 * Methods for allowing movement only when pressing the Entity's rectangle.
 * Otherwise, the whole boundingRect is a posible selection area.
 */
void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF rect(QPointF(scenePos()),QPointF(scenePos()+QPointF(width,height)));
    if (rect.contains(event->scenePos())){
        isMoving = true;
        QGraphicsItem::mousePressEvent(event);
    }
    else{
        event->ignore();
        this->setSelected(false);
    }
}
void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isMoving) QGraphicsItem::mouseMoveEvent(event);
}
void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isMoving) QGraphicsItem::mouseReleaseEvent(event);
    isMoving = false;
}
