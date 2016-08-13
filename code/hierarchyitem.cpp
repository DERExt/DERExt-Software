#include "hierarchyitem.h"
#include "entityitem.h"
#include "qdialog.h"
#include "QtCore"
#include "QtGui"
HierarchyItem::HierarchyItem(GraphicView * graphicsView, Error * error, QWidget * parent,
                            QString typeName, bool exclusive, bool total, QList<EntityItem*> ents)
{
    if(ents.size() > 2){
        this->graphicView = graphicsView;
        this->error = error;
        this->parent = parent;
        this->entities = ents;
        this->exclusive = exclusive;
        this->total = total;
        this->typeName = typeName;
        this->name = "Hierarchy " + ents.at(0)->getName();
        isMoving = false;
        this->setFlag(ItemIsMovable, true);
        this->setFlag(ItemSendsGeometryChanges,true);
        this->setFlag(ItemIsSelectable,true);
        QList<Entity *> entitiesNoItem;
        foreach(EntityItem* item, entities){
            item->addHierarchy(this);
            entitiesNoItem << (Entity*)item->getERItem();
        }
        hierarchy = new Hierarchy(typeName, exclusive, total, error, entitiesNoItem);
    }
}

bool HierarchyItem::getExclusive()
{
    return this->exclusive;
}

bool HierarchyItem::getTotal()
{
    return this->total;
}

QString HierarchyItem::getTypeName()
{
    return this->typeName;
}

void HierarchyItem::setTypeName(QString typeName)
{
    this->typeName = typeName;
}

QString HierarchyItem::getName()
{
    return name;
}

void HierarchyItem::setName(QString name)
{
    this->name = "Hierarchy " + name;;
}

bool HierarchyItem::addAttribute(Attribute *attribute, QString parent)
{
    /*No implementar, no posee*/
}

bool HierarchyItem::removeAttribute(QString attributesName)
{
    /*No implementar, no posee*/
}

QList<Attribute *> HierarchyItem::getAllAttributes()
{
    /*No implementar, no posee*/
}

QList<Attribute *> HierarchyItem::getAttributes()
{
    /*No implementar, no posee*/
}

void HierarchyItem::setPosition()
{

}

void HierarchyItem::setPosition(QPointF pos)
{

}

void HierarchyItem::setRelationship(Hierarchy *rship)
{
    /*No implementar, no posee*/
}

QString HierarchyItem::getType()
{

}

int HierarchyItem::type() const
{
    return Type;
}

void HierarchyItem::setGraphicView(GraphicView *view)
{
    this->graphicView = view;
}

GraphicView *HierarchyItem::getGraphicView()
{
    return this->graphicView;
}

QList<EntityItem *> HierarchyItem::getEntities()
{
    return this->entities;
}

QList<Cardinality *> HierarchyItem::getCardinalities()
{

}

bool HierarchyItem::moveAttribute(QString attribute, int row, int pos, QTableWidget *table)
{

}

ERItem *HierarchyItem::getERItem()
{

}
QVariant HierarchyItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneChange){
        if (this->scene() == NULL)          //agregado por primera vez
            reloadEntities();
        else
            removeFromEntities();
    }
    return QGraphicsItem::itemChange(change,value);
}

void HierarchyItem::reloadEntities()
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
            item->addHierarchy(this);

        /*QList<Entity*> ents;
        foreach(EntityItem * ent, entities)
            ents << (Entity*)ent->getERItem();
        if (entities.size() == 1)       //unary rship
            ents << (Entity*)entities.at(0)->getERItem();
        hrchy->setEntities(ents);*/
    }
}

void HierarchyItem::removeFromEntities()
{
    foreach(EntityItem* item, entities){
        item->removeHierarchy(this);
    }
}

IGraphicItem *HierarchyItem::getCopy()
{
    HierarchyItem *item;
    item = new HierarchyItem(graphicView, error, parent, typeName, exclusive, total, entities);
    return item;
}

void HierarchyItem::adjust()
{

}

QString HierarchyItem::getDerivation()
{

}

void HierarchyItem::getXML(QDomDocument *document, QDomElement *root)
{
    QDomElement * tmp = (hierarchy->getXML(document, "Hierarchy"));
    root->appendChild(*tmp);
}

QRectF HierarchyItem::boundingRect() const
{
    QPointF a(0.0, 0.0);
    QSizeF b(36.0, 36.0);
    //X minima
    int minX = entities.at(0)->x();
    foreach(EntityItem * item, entities)
        if(item->x() < minX)
            minX = item->x();
    //Y minima
    int minY = entities.at(0)->y();
    foreach(EntityItem * item, entities)
        if(item->y() < minY)
            minY = item->y();
    //X maxima
    int maxX = entities.at(0)->x();
    foreach(EntityItem * item, entities)
        if(item->x()+ item->getWidth() > maxX)
            maxX = item->x() + item->getWidth();
    //Y maxima
    int maxY = entities.at(0)->y();
    foreach(EntityItem * item, entities)
        if(item->y()+ item->getHeight() > maxY)
            maxY = item->y() + item->getHeight();
    return QRectF(QPoint(minX-30,minY-30),QPoint(maxX+30,maxY+30));
}

void HierarchyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    prepareGeometryChange(); //permite ver como se mueve en tiempo real
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QRectF base = boundingRect();
    QPen pen = painter->pen();
    painter->setBrush(Qt::white);

    /*if (this->isSelected()){
        painter->setPen(Qt::DashLine);
        painter->drawRect(boundingRect());
    }*/
    //painter->drawRect(base);
    QPointF puntoElipse(base.x()+base.width()/2, base.y()+base.height()/2);

    /*Conecta las entidades*/
    float elipseDim = 35/2;
    QRectF elipseRec(QPointF(puntoElipse.x()-elipseDim,puntoElipse.y()-elipseDim), QSizeF(35, 35));
    foreach(EntityItem * item, entities){
        //QPointF pto =
        QPointF pto = rectangleCenter(item);
        painter->drawLine(puntoElipse, pto);
    }
    painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if(total){
       QPointF punto(4, 4);
       QPointF punto2(2, 2);
       painter->drawLine(puntoElipse.operator +=(punto), rectangleCenter(entities.at(0)).operator +=(punto2));
       painter->drawLine(puntoElipse.operator -=(punto), rectangleCenter(entities.at(0)).operator -=(punto2));
    }
    else
        painter->drawLine(puntoElipse, rectangleCenter(entities.at(0)));
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawEllipse(elipseRec);
    if(exclusive){
        painter->drawText(elipseRec,Qt::AlignCenter, "d");
        painter->drawText((puntoElipse.x()+entities.at(0)->x())/2+15,puntoElipse.y()-25, this->typeName);
    }
    else
        painter->drawText(elipseRec,Qt::AlignCenter, "O");
}

QPointF *HierarchyItem::intersect(QList<QLineF> lines, QLineF line)
{

}

QPointF *HierarchyItem::intersectRect(QRectF rect, QLineF line)
{

}

QPointF HierarchyItem::rectangleCenter(EntityItem *item)
{
    qreal centerX = (item->pos().x() + item->getWidth() /2);
    qreal centerY = (item->pos().y() + item->getHeight()/2);

    return QPointF(centerX,centerY);
}



void HierarchyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    isMoving = true;
    update();
    QGraphicsItem::mousePressEvent(event);
}

void HierarchyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    /*if (isMoving)
       QGraphicsItem::mouseMoveEvent(event);*/
}

void HierarchyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    isMoving = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);

}

void HierarchyItem::load(QDomElement *e)
{

}
