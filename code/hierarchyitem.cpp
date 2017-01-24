#include "hierarchyitem.h"
#include "entityitem.h"
#include "qdialog.h"
#include "QtCore"
#include "QtGui"
HierarchyItem::HierarchyItem(GraphicView * graphicsView, Error * error, QWidget * parent,
                            QString typeName, bool exclusive, bool total, QList<EntityItem*> ents)
{
    if(ents.size() > 1){
        this->graphicView = graphicsView;
        this->error = error;
        this->parent = parent;
        this->entities = ents;
        this->typeName = typeName;
        this->name = "Hierarchy " + ents.at(0)->getName();
        this->setFlag(ItemIsMovable, false);
        this->setFlag(ItemSendsGeometryChanges,false);
        this->setFlag(ItemIsSelectable,false);
        QList<Entity *> entitiesNoItem;
        entities.at(0)->setHierarchy(this);
        foreach(EntityItem* item, entities){
            entitiesNoItem << (Entity*)item->getERItem();
        }
        for(int i = 1; i < entities.count(); i++)
            entities.at(i)->setHierarchySon(this);
        hierarchy = new Hierarchy(typeName, exclusive, total, error, entitiesNoItem);
    }
}

bool HierarchyItem::getExclusive()
{
    return this->hierarchy->isExclusive();
}

bool HierarchyItem::getTotal()
{
    return this->hierarchy->isTotal();
}

QString HierarchyItem::getTypeName()
{
    return this->hierarchy->getTypeName();
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

bool HierarchyItem::addAttribute(Attribute *attribute, QString parent){}

bool HierarchyItem::removeAttribute(QString attributesName){}

QList<Attribute *> HierarchyItem::getAllAttributes(){}

QList<Attribute *> HierarchyItem::getAttributes(){}

void HierarchyItem::setPosition(){}

void HierarchyItem::setPosition(QPointF pos){}

void HierarchyItem::setRelationship(Hierarchy *rship){}

QString HierarchyItem::getType(){
    return QString("Hierarchy");
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

QList<Cardinality *> HierarchyItem::getCardinalities(){}
bool HierarchyItem::moveAttribute(QString attribute, int row, int pos, QTableWidget *table){}

ERItem *HierarchyItem::getERItem()
{
    return this->hierarchy;
}
QVariant HierarchyItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneChange){
        if (this->scene() == NULL)
            reloadEntities();
        else{
            removeFromEntities();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

void HierarchyItem::reloadEntities()
{
    if (graphicView != NULL){
        QList<EntityItem*> result;
        foreach(EntityItem* item , entities)
            result.push_back((EntityItem*)this->graphicView->getItem(item->getName()));
        entities.clear();
        entities.append(result);
        removeFromEntities();
        entities.at(0)->setHierarchy(this);
        for(int i = 1; i < entities.count(); i++)
            entities.at(i)->setHierarchySon(this);
    }
}

void HierarchyItem::removeFromEntities()
{
    entities.at(0)->RemoveHierarchy();
    for(int i = 1; i < entities.count(); i++)
        entities.at(i)->RemoveHierarchySon();
}

IGraphicItem *HierarchyItem::getCopy()
{
    HierarchyItem *item;
    item = new HierarchyItem(graphicView, error, parent, typeName, hierarchy->isExclusive(), hierarchy->isTotal(), entities);
    return item;
}

QString HierarchyItem::getDerivation()
{
    return this->hierarchy->getDerivation();
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
    updateEntities();
    prepareGeometryChange(); //permite ver como se mueve en tiempo real
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QRectF base = boundingRect();
    QPen pen = painter->pen();
    painter->setBrush(Qt::white);
    QPointF puntoElipse(base.x()+base.width()/2, base.y()+base.height()/2);

    /*Conecta las entidades*/
    float elipseDim = 35/2;
    QRectF elipseRec(QPointF(puntoElipse.x()-elipseDim,puntoElipse.y()-elipseDim), QSizeF(35, 35));
    foreach(EntityItem * item, entities){
        QPointF pto = rectangleCenter(item);
        painter->drawLine(puntoElipse, pto);
    }
    painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if(hierarchy->isTotal()){
       QPointF punto(4, 4);
       QPointF punto2(2, 2);
       painter->drawLine(puntoElipse.operator +=(punto), rectangleCenter(entities.at(0)).operator +=(punto2));
       painter->drawLine(puntoElipse.operator -=(punto), rectangleCenter(entities.at(0)).operator -=(punto2));
    }
    else
        painter->drawLine(puntoElipse, rectangleCenter(entities.at(0)));
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawEllipse(elipseRec);
    if(hierarchy->isExclusive()){
        painter->drawText(elipseRec,Qt::AlignCenter, "d");
        painter->drawText((puntoElipse.x()+entities.at(0)->x())/2+15,puntoElipse.y()-25, "  "+this->typeName);
    }
    else
        painter->drawText(elipseRec,Qt::AlignCenter, "O");
}

QPointF HierarchyItem::rectangleCenter(EntityItem *item)
{
    qreal centerX = (item->pos().x() + item->getWidth() /2);
    qreal centerY = (item->pos().y() + item->getHeight()/2);

    return QPointF(centerX,centerY);
}

void HierarchyItem::load(QDomElement *e){}

void HierarchyItem::removeEntity(EntityItem *item)
{
    if(item != NULL)
        entities.removeAll(item);
}

void HierarchyItem::updateEntities(){
    QList<EntityItem*> aux;
    QList<Entity*> auxNoItem;
    foreach(EntityItem* item, entities){
        if(graphicView->getItem(item->getName()) != NULL){
            EntityItem *entAux =((EntityItem*)graphicView->getItem(item->getName()));
            aux << entAux;
            auxNoItem << ((Entity*)entAux->getERItem());
        }
    }
    entities.clear();
    entities.append(aux);
    hierarchy->setEntities(auxNoItem);
}
