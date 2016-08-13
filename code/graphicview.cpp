#include "graphicview.h"
#include <QDebug>

GraphicView::GraphicView(QWidget *parent):QGraphicsView(parent){
    QGraphicsScene *scene = new QGraphicsScene(this);
    this->setScene(scene);
    scene->setSceneRect(this->frameRect());
    path.clear();
    this->setName();
    this->saved = true;
    selection = NULL;
}


void GraphicView::setPath(QString path){
    this->path = path;
    this->setName();
}

QString GraphicView::getPath() const
{
    return this->path;
}

QString GraphicView::getName() const
{
    return (QFileInfo(path).fileName());
}

bool GraphicView::isSaved() const
{
    return this->saved;
}

void GraphicView::setSaved(bool saved)
{
    this->saved = saved;
    if(saved){
        QGraphicsScene *scene = this->scene();
        if(scene){
            QList<QGraphicsItem*> items = scene->items();
            if(!items.empty()){
                foreach(QGraphicsItem* item, items)
                    ((IGraphicItem*)item)->setPosition();
            }
        }
    }
    this->setName();
}

bool GraphicView::saveView()
{
    bool ok = false;
    if(path.isEmpty())
        this->saveAs(ok);
    else this->save(ok);
    return ok;
}

void GraphicView::save(bool &ok)
{
    QFile file(path+".dere");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        QDomDocument document;
        QDomElement root;
        root = document.createElement("DERExt");
        QList<QGraphicsItem*> items = this->scene()->items();
        foreach(QGraphicsItem* item, items)
            ((IGraphicItem*)item)->getXML(&document, &root);
        document.appendChild(root);
        stream << document.toString();
        file.close();
        this->setSaved(true);
        ok=true;
    }
    else ok=false;
}

void GraphicView::saveAs(bool &ok)
{
    QString filter = "DERExt (*.dere)";
    path = QFileDialog::getSaveFileName(this, "Save files", QDir::currentPath());
    if(!path.isEmpty())
        this->save(ok);
}

IGraphicItem *GraphicView::getItem(QString name)
{
    foreach(QGraphicsItem * item, this->items()){
        if (((IGraphicItem*)item)->getName() == name){
            return (IGraphicItem*)item;
        }
    }
    return NULL;
}

QMenu *GraphicView::buildAlignMenu()
{
    QMenu * menu = new QMenu(this);

    QAction * topAction = new QAction("Top",menu);
    connect(topAction,SIGNAL(triggered(bool)),this,SLOT(alignTop()));
    menu->addAction(topAction);
    QAction * bottomAction = new QAction("Bottom",menu);
    connect(bottomAction,SIGNAL(triggered(bool)),this,SLOT(alignBottom()));
    menu->addAction(bottomAction);
    QAction * leftAction = new QAction("Left",menu);
    connect(leftAction,SIGNAL(triggered(bool)),this,SLOT(alignLeft()));
    menu->addAction(leftAction);
    QAction * rightAction = new QAction("Right",menu);
    connect(rightAction,SIGNAL(triggered(bool)),this,SLOT(alignRight()));
    menu->addAction(rightAction);
    return menu;
}

/**
 * @brief GraphicView::getNewComponent
 * @param alignX [flag para determinar si se debe alinear respecto al eje X o al eje Y]
 * @param min    [flag que determina si se alinea respecto al mínimo respecto al componente dado]
 * @return position [nueva posición del eje X o del eje Y]
 */

int GraphicView::getNewComponent(bool alignX, bool min) {
    int position = (min) ? INT_MAX : INT_MIN;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if (min){
            if (alignX){
                if (item->x() < position)
                    position = item->x();
            }
            else if (item->y() < position)
                    position = item->y();
        }
        else{
            if (alignX){
                if (item->x() > position)
                    position = item->x();
            }
            else if (item->y() > position)
                    position = item->y();
        }
    }

    return position;
}

QGraphicsItem * GraphicView::getMostLeftComponent() {
    int left = INT_MAX; //Máximo de la pantalla para comenzar el greedy.
    QGraphicsItem * mostLeftItem;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->x() < left) {
            mostLeftItem = item;
            left = item->x();
        }
    }
    return mostLeftItem;
}

QGraphicsItem * GraphicView::getMostRightComponent() {
    int right = INT_MIN;
    QGraphicsItem * mostRightItem;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->x() > right) {
            mostRightItem = item;
            right = item->x();
        }
    }
    return mostRightItem;
}


/**
 * ELE EJE y ESTÁ INVERTIDO.
 */

QGraphicsItem * GraphicView::getMostTopComponent() {
    int top = INT_MAX;
    QGraphicsItem * mostTopItem;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->y() < top) {
            mostTopItem = item;
            top = item->y();
        }
    }
    return mostTopItem;
}

QGraphicsItem * GraphicView::getMostBottomComponent() {
    int bottom = INT_MIN;
    QGraphicsItem * mostBottomItem;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->y() > bottom) {
            mostBottomItem = item;
            bottom = item->y();
        }
    }
    return mostBottomItem;

}


/**
 * @brief GraphicView::setNewPosition
 * @param alignX [determina si el elemento se alinea según el eje X o el Y]
 * @param newComponent [nuevo componente en X o Y]
 * @param offset [si se alinea por el eje X, entonces este parámetro se suma a la posición en Y y viceversa]
 * @param item [el elemento al cual se redefine uno de sus componentes de coordenada]
 */

void GraphicView::setNewPosition(bool alignX, int newComponent, int offset, QGraphicsItem * item) {
    if (alignX) {
        item->setX(newComponent);
        //Si bien se alinea en el eje X, el eje Y cambia para que no quede todo junto.
        item->setY(offset);
    }
    else {
        //IDEM.
        item->setX(offset);
        item->setY(newComponent);
    }
}


/**
 * @brief GraphicView::getDistance
 * @param from      [elemento base]
 * @param to        [elemento al cual se desea medir la distancia]
 * @return          [distancia entre from y to]
 */

int GraphicView::getDistance(QGraphicsItem * from, QGraphicsItem * to) {
    int endFromX = from->x() + from->boundingRect().width(),
        endFromY = from->y() + from->boundingRect().height();

   return sqrt(pow(to->x() - endFromX, 2) + pow(to->y() - endFromY, 2));
}

void GraphicView::align(bool alignX, bool min) {
    int i = 0,
        newPosition = 0,
        distance = 0,
        base;

    QGraphicsItem * previousItem;


    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        qDebug() << "SE ENCUENTRA EN (" << item->x() << "," << item->y() << ")";
        base = (alignX) ? item->y() : item->x();
        distance = (i == 0) ? base : this->getDistance(previousItem, item) / 2;
        newPosition = this->getNewComponent(alignX, min);
        this->setNewPosition(alignX, newPosition, distance, item);
        previousItem = item;
        i++;
    }
}

void GraphicView::alignBottom(){
    QGraphicsItem * mostBottom = this->getMostBottomComponent(); //Inmovil
    qDebug() << mostBottom->y();
    int baseY = mostBottom->y() - mostBottom->boundingRect().height();
    int baseX;
    int distance;

    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->y() != mostBottom->y() && item != mostBottom) {
            baseX = (mostBottom->x() < item->x()) ? mostBottom->x() + mostBottom->boundingRect().width() : mostBottom->x();
            distance = sqrt(pow(item->x() - baseX, 2) + pow(item->y() - baseY, 2));
            item->setY(mostBottom->y());

            if(item->x() > mostBottom->x())
                item->setX(baseX + distance);
            else
                item->setX(baseX - distance);
        }
    }
}

void GraphicView::alignTop(){
    QGraphicsItem * mostTop = this->getMostTopComponent(); //Inmovil
    qDebug() << mostTop->y();
    int baseY = mostTop->y();
    int baseX;
    int distance;

    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->y() != mostTop->y() && item != mostTop) {
            baseX = (mostTop->x() < item->x()) ? mostTop->x() + mostTop->boundingRect().width() : mostTop->x();
            distance = sqrt(pow(item->x() - baseX, 2) + pow(item->y() - baseY, 2));
            item->setY(mostTop->y());

            if(item->x() > mostTop->x())
                item->setX(baseX + distance);
            else
                item->setX(baseX - distance);
        }
    }
}




void GraphicView::alignLeft(){
    QGraphicsItem * mostLeft = this->getMostLeftComponent(); //Inmovil

    int baseX = mostLeft->x() + mostLeft->boundingRect().width();
    int baseY;
    int distance;

    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->x() != mostLeft->x() && item != mostLeft) {
            baseY = (mostLeft->y() < item->y()) ? mostLeft->y() : mostLeft->y() - mostLeft->boundingRect().height();
            distance = sqrt(pow(item->x() - baseX, 2) + pow(item->y() - baseY, 2));
            item->setX(mostLeft->x());
            if(item->y() > mostLeft->y())
                item->setY(baseY + distance);
            else
                item->setY(baseY - distance);
        }
    }
}

void GraphicView::alignRight(){
    QGraphicsItem * mostRight = this->getMostRightComponent(); //Inmovil, puesto que es el que está más a la izquierda.

    int baseX = mostRight->x();
    int baseY;
    int distance;

    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if(item->x() != mostRight->x() && item != mostRight) {
            baseY = (mostRight->y() < item->y()) ? mostRight->y() : mostRight->y() - mostRight->boundingRect().height();
            distance = sqrt(pow(item->x() - baseX, 2) + pow(item->y() - baseY, 2));
            item->setX(mostRight->x());
            if(item->y() > mostRight->y())
                item->setY(baseY + distance);
            else
                item->setY(baseY - distance);
        }
    }
}

void GraphicView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (this->scene()->selectedItems().size() > 1){
        buildAlignMenu()->exec(event->screenPos().toPoint());
        foreach(QGraphicsItem * item, this->scene()->selectedItems())
            item->setSelected(false);
    }
    qDebug() << "SOLTÉ EL MOUSE";
}

void GraphicView::mousePressEvent(QMouseEvent *event)
{
    foreach(QGraphicsItem * item, this->scene()->selectedItems())
        item->setSelected(false);
    QGraphicsView::mousePressEvent(event);
}

void GraphicView::setName()
{
    if(!path.isEmpty())
    {
        QFileInfo *name = new QFileInfo(path);
        if(saved)
            setWindowTitle(name->fileName());
        else setWindowTitle(name->fileName()+"*");
    }
    else {
        if(saved)
            setWindowTitle("New");
        else setWindowTitle("New*");
    }
}
