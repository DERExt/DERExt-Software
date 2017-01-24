#include "entityitem.h"
#include "weakentity.h"

#include <QRectF>
#include <QLineF>
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>

#include <QDebug>

QRectF EntityItem::getRectangle() {

    return (this->isAssociation)
            ?   QRectF(
                    QPointF(this->getMostLeftComponent()->x() - 10,this->getMostTopComponent()->y() - 10),
                    QPointF(this->getMostRightComponent()->x() + 10, this->getMostBottomComponent()->y() + 10)
                )
            :   QRectF(
                    QPointF(scenePos()),
                    QPointF(scenePos()+QPointF(width,height))
                );
}

void EntityItem::setBounds(QString entityName) {
    if (! this->isAssociation) {
        this->width = entityName.count()*charWidth + 20;
        this->height = 40;
    }
}

void EntityItem::printElements() {
    for (EntityItem * item : this->aggregationRel->getEntities()) {
        qDebug() << item->pos();
    }
}

QPointF * EntityItem::getMostLeftComponent() {
    int left = INT_MAX; //Máximo de la pantalla para comenzar el greedy.
    QGraphicsItem * mostLeftItem;
    foreach(QGraphicsItem * item, this->aggregationRel->getEntities()){
        if(item->x() < left) {
            mostLeftItem = item;
            left = item->x();
        }
    }
    QPointF * point = new QPointF(mostLeftItem->pos());
    return point;
}

QPointF * EntityItem::getMostRightComponent() {
    int right = INT_MIN;
    QGraphicsItem * mostRightItem;
    foreach(QGraphicsItem * item, this->aggregationRel->getEntities()){
        if(item->x() > right) {
            mostRightItem = item;
            right = item->x();
        }
    }
    QPointF * point = new QPointF(mostRightItem->pos().x() + mostRightItem->boundingRect().width(), mostRightItem->pos().y());
    return point;
}


/**
 * ELE EJE y ESTÁ INVERTIDO.
 */

QPointF * EntityItem::getMostTopComponent() {
    int top = INT_MAX;
    QGraphicsItem * mostTopItem;
    foreach(QGraphicsItem * item, this->aggregationRel->getEntities()){
        if(item->y() < top) {
            mostTopItem = item;
            top = item->y();
        }
    }
    QPointF * point = new QPointF(mostTopItem->pos());
    return point;
}

QPointF * EntityItem::getMostBottomComponent() {
    int bottom = INT_MIN;
    QGraphicsItem * mostBottomItem;
    foreach(QGraphicsItem * item, this->aggregationRel->getEntities()){
        if(item->y() > bottom) {
            mostBottomItem = item;
            bottom = item->y();
        }
    }
    QPointF * point = new QPointF(mostBottomItem->pos().x(), mostBottomItem->pos().y() + mostBottomItem->boundingRect().height());
    return point;
}

EntityItem::EntityItem(QString entityName, QDomElement *element, GraphicView *graphicView, Error *error, QWidget *parent, bool sub,
                       bool old, bool weak, Entity * strongEnt, bool isAssociation, RelationshipItem * rel)
{
    this->isAssociation = isAssociation;
    this->aggregationRel = rel;

    //DIBUJAR LA ASOCIACIÓN O LA ENTIDAD [OTRA FUNCIÓN].
    this->setBounds(entityName);
    this->graphicView = graphicView;
    this->error = error;
    this->parent = parent;
    this->isWeak = weak;
    this->isMoving = false;
    this->old = old;
    if (isWeak)
        this->ent = new WeakEntity(entityName, this->error, strongEnt);
    else {
        if(!isAssociation) {
            this->ent = new Entity(entityName, this->error);
        }
        else {
            this->ent = new Entity(entityName, this->error, isAssociation, (Relationship*)rel->getERItem());
            this->ent->setAssociation(rel->getRship());
        }
    }
    ent->setSubtype(sub);
    ent->setOldState(old);
    this->subtype = false;
    this->subtype = sub;
    this->setFlag(ItemIsMovable, true);
    this->setFlag(ItemSendsGeometryChanges,true);
    this->setFlag(ItemIsSelectable,true);
    this->loadNode(element);
    this->ent->generateCompSK();
    if(graphicView)
        this->graphicView->setSaved(true);
    if(!isAssociation)
        this->setPosition();
    else
        rel->setPartOfAssociation();
        //this->setPosition(QPointF(this->getMostLeftComponent()->x() - 10, this->getMostTopComponent()->y() - 10));
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
/*//////////*/       newAtt = new AttSimple(element.attribute("name"), element.attribute("null").toInt(), element.attribute("mult").toInt(), element.attribute("pk").toInt(), element.attribute("sk").toInt(), bprnt);
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

void EntityItem::setAssociation(RelationshipItem * rel)
{
    this->ent->setAssociation(rel->getRship());
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

    if(this->isAssociation) {
        tmp->setAttribute("x", this->getMostLeftComponent()->x() - 10);
        tmp->setAttribute("y", this->getMostTopComponent()->y() - 10);
    }
    else {
        tmp->setAttribute("x", this->pos().x());
        tmp->setAttribute("y", this->pos().y());
    }

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
        tmp = new EntityItem(ent->getName(), NULL, this->graphicView, this->error, this->parent, this->subtype, old,isWeak,
                             ((WeakEntity*)ent)->getStrongEntity());
    else tmp = new EntityItem(ent->getName(), NULL, this->graphicView, this->error, this->parent, this->subtype, old);
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

bool EntityItem::isSubtype()
{
    return ent->isSubtype();
}

void EntityItem::setSubtype(bool op)
{
    subtype = op;
    ent->setSubtype(op);
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

void EntityItem::removeRelationship(RelationshipItem *item)
{
    rships.removeAll(item);
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

void EntityItem::setOldState(bool state){
    this->ent->setOldState(state);
}

bool EntityItem::getHasIdDependency()
{
    foreach(RelationshipItem * r,rships)
        if (r->getIdDependency())
            return true;
    return false;
}

QPointF EntityItem::getRectangleCenter() {

    QRectF rect = QRectF(
                QPointF(this->getMostLeftComponent()->x() - 10,this->getMostTopComponent()->y() - 10),
                QPointF(this->getMostRightComponent()->x() + 10, this->getMostBottomComponent()->y() + 10));
    this->centerRect = rect.center();

    return (this->isAssociation) ? this->centerRect : QPointF(0,0);
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

    if(this->isAssociation)
        this->setZValue(-100);

    painter->setBrush(Qt::transparent);

    if (this->isSelected() && !this->isAssociation)
            painter->setPen(Qt::DashLine);

    if (this->isAssociation) {
        QRectF rect = QRectF(
                    QPointF(this->getMostLeftComponent()->x() - 10,this->getMostTopComponent()->y() - 10),
                    QPointF(this->getMostRightComponent()->x() + 10, this->getMostBottomComponent()->y() + 10));
        this->centerRect = rect.center();
        painter->drawRect(rect);

        QLineF l1(QPointF(this->getMostLeftComponent()->x() - 10, this->getMostTopComponent()->y() - 10),
                  QPointF(this->getMostLeftComponent()->x() - 10, this->getMostBottomComponent()->y() + 10));

        QLineF l2(QPointF(this->getMostLeftComponent()->x() - 10, this->getMostTopComponent()->y() - 10),
                  QPointF(this->getMostRightComponent()->x() + 10, this->getMostTopComponent()->y() -10));

        this->width = l1.length();
        this->height = l2.length();
    }
    else
        painter->drawRect(QRectF(QPointF(0,0),QPointF(width,height)));

    if (isWeak)
        painter->drawRect(QRectF(QPointF(3,3),QPointF(width-3,height-3)));
    if(!this->isAssociation)
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

HierarchyItem* EntityItem::getHierarchy(){
    return fatherH;
}

HierarchyItem* EntityItem::getHierarchySon(){
    return sonH;
}

void EntityItem::setHierarchy(HierarchyItem *item){
    fatherH = item;
    ent->setHierarchy((Hierarchy*)item->getERItem());
}

void EntityItem::setHierarchySon(HierarchyItem *item){
    sonH = item;
    ent->setHierarchySon((Hierarchy*)item->getERItem());
}

bool EntityItem::isFather() const{
    return ent->isFather();
}

void EntityItem::RemoveHierarchy(){
    fatherH = NULL;
    this->ent->RemoveHierarchy();
}

void EntityItem::RemoveHierarchySon(){
    sonH = NULL;
    this->ent->RemoveHierarchySon();
}

QVariant EntityItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if  (change == ItemSceneHasChanged && this->scene()==NULL){
        foreach(RelationshipItem * rship, rships){
            rship->removeFromEntities();
            if (rship->scene()!=NULL)
                rship->scene()->removeItem(rship);
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
void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QRectF rect(QPointF(scenePos()),QPointF(scenePos()+QPointF(width,height)));
    if (rect.contains(event->scenePos()) && !this->isAssociation){
        isMoving = true;
        QGraphicsItem::mousePressEvent(event);
    }
    else {
        event->ignore();
        this->setSelected(false);
    }
}

void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isMoving && !this->isAssociation)
        QGraphicsItem::mouseMoveEvent(event);
}

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (isMoving) {
        this->scene()->update();
        this->scene()->update(this->sceneBoundingRect());
        QGraphicsItem::mouseReleaseEvent(event);

    }
    isMoving = false;
}
