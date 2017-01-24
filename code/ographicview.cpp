#include "graphicview.h"


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

void GraphicView::align(bool alignX, bool min)
{
    int position = (min) ? INT_MAX : INT_MIN;
    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if (min){
            if (alignX){
                if (item->x() < position)
                    position = item->x();}
            else if (item->y() < position)
                    position = item->y();
        }
        else{
            if (alignX){
                if (item->x() > position)
                    position = item->x();}
            else if (item->y() > position)
                    position = item->y();
        }
    }

    foreach(QGraphicsItem * item, this->scene()->selectedItems()){
        if (alignX)
            item->setX(position);
        else item->setY(position);
    }
}

//"trampoline" methods because of not being able to pass parameters on connect
void GraphicView::alignTop(){ align(false,true); }

void GraphicView::alignBottom(){ align(false,false); }

void GraphicView::alignLeft(){ align(true,true); }

void GraphicView::alignRight(){ align(true,false); }

void GraphicView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (this->scene()->selectedItems().size() > 1){
        buildAlignMenu()->exec(event->screenPos().toPoint());
        foreach(QGraphicsItem * item, this->scene()->selectedItems())
            item->setSelected(false);
    }
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
