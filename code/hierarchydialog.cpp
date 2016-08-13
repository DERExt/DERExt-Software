#include "hierarchydialog.h"
#include "ui_hierarchydialog.h"
#include "dibujar.h"
#include "rternaryitem.h"

HierarchyDialog::HierarchyDialog(QList<QGraphicsItem*> items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HierarchyDialog)
{
    error = new Error(this);
    sceneItems.append(items);
    ui->setupUi(this);
    ui->lineEdit->setDisabled(true);
    foreach(QGraphicsItem * item, sceneItems){
        //if (item->type() == EntityItem::Type){
            //ui->sup->addItem(((IGraphicItem*)item)->getName());
            //QString name = ((IGraphicItem*)item)->getName();
            //ui->listEnt->insertItem(0, name);
        //}
        backup.append(((IGraphicItem*)item)->getCopy());
    }

    /*crear o modificar*/
    ui->modeComboBox->addItem("Create");
    ui->modeComboBox->addItem("Modify");
    this->mode = Mode(ui->modeComboBox->currentIndex());
}

void HierarchyDialog::on_rtypeComboBox_activated(const QString &arg1)
{

}



void HierarchyDialog::on_buttonBox_accepted()
{

    if (ui->exclusiveBool->checkState()){
        if (ui->lineEdit->text().isEmpty()){
            error->setError(Error::TypeHrchy);
            this->show();
        }
    }
    int g = ui->listSub->count();
    for(int j = 0; j < g ; j++){
        if(ui->listSub->item(j)->text() == ui->sup->currentText()){
            error->setError(Error::NameSub);
            this->show();
            return;
        }
    }
    if(ui->listSub->count() > 1 ){
        if(mode == HierarchyDialog::Modify){
            auxModify = getHierarchy()->getEntities().at(0);
            MainWindow * window = (MainWindow*)this->parentWidget();
            window->deleteItem(getHierarchy());
            sceneItems.removeOne(getHierarchy());
        }
        MainWindow * window = (MainWindow*)this->parentWidget();
        HierarchyItem * item = createHierarchy();
        window->addItem(item);
        error->setError(Error::NoError);
        this->close();
    }else{
         error->setError(Error::NoEntity);
        this->show();
        return;
    }
}

void HierarchyDialog::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, sceneItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup)
        main->addItem((IGraphicItem*)item);
    this->close();
}

HierarchyDialog::~HierarchyDialog()
{
    delete ui;
}
void HierarchyDialog::on_exclusiveBool_clicked(bool checked)
{

    if(checked){
        ui->exclusiveBool->setCheckState(Qt::Checked);
        ui->lineEdit->setDisabled(false);
        if(mode == HierarchyDialog::Modify)
            ui->lineEdit->setText(getHierarchy()->getTypeName());
    }
    else
        ui->lineEdit->setDisabled(true);
}

void HierarchyDialog::on_HierarchyDialog_destroyed()
{

}

void HierarchyDialog::on_checkBox_clicked()
{

}

void HierarchyDialog::on_pushAdd_clicked()
{
    if(ui->listEnt->currentItem() != NULL){
        int row = ui->listEnt->currentRow();
        QString name =  ui->listEnt->currentItem()->text();
        ui->listSub->insertItem(row, name);
        delete ui->listEnt->currentItem();
    }
    else return;
}

void HierarchyDialog::on_pushRemove_clicked()
{
    if(ui->listSub->currentItem() != NULL){
        int row = ui->listSub->row(ui->listSub->currentItem());
        QString name =  ui->listSub->currentItem()->text();
        ui->listEnt->insertItem(row, name);
        delete ui->listSub->currentItem();
    }
    else return;

}

HierarchyItem *HierarchyDialog::createHierarchy()
{
    QList<EntityItem *> entities;
    int longList = ui->listSub->count();
    if(mode == HierarchyDialog::Create){
        foreach(QGraphicsItem * item, sceneItems){
            if (item->type() == EntityItem::Type){
                    if(ui->sup->currentText() == ((IGraphicItem*)item)->getName()){
                        entities << (((EntityItem*)item));
                }
            }
        }
    }
    else{
        if(mode == HierarchyDialog::Modify){
            entities << auxModify;
            foreach(EntityItem *item, entities){
                item->deleteFather();
            }
        }
    }
    foreach(QGraphicsItem * item, sceneItems){
        if (item->type() == EntityItem::Type){
            for(int i = 0; i < longList ; i++){
                if(ui->listSub->item(i)->text() == ((IGraphicItem*)item)->getName()){
                    entities << (((EntityItem*)item));
                    ((EntityItem*)item)->setFather(entities.at(0));
                }
            }
        }
    }

        bool exc = ui->exclusiveBool->checkState();
        bool tot = ui->totalBool->checkState();
        MainWindow * window = (MainWindow*)this->parentWidget();
        return new HierarchyItem(NULL,error,window, ui->lineEdit->text(), exc ,tot, entities);

}

HierarchyItem *HierarchyDialog::getHierarchy()
{
    if (mode == HierarchyDialog::Modify){
        QString currentName = ui->sup->currentText();
        if (currentName.isEmpty()) return NULL;

        foreach(QGraphicsItem * item, sceneItems){
            if (((IGraphicItem*)item)->getName() == currentName)
                return (HierarchyItem*)item;
        }
    }
}



/*void HierarchyDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //ui->setupUi(this);
    //QGraphicsItem a = new QGraphicsItem();

    QPen framePen(Qt::black);
    framePen.setWidth(3);/*Grosor el lapiz*/
/*    painter.setPen(framePen);

    QPoint posR(180, 150);/*Posicion X e Y del centro de la entidad.*/
/*    QSize dimR(60,35);/*Dimensiones del circulo*/
/*    QRect recR(posR, dimR);
    painter.drawRect(recR);
    QPoint d(recR.center());
    d.setY(d.y()+ recR.height()/2+2);


    QPoint pos(195, 203);/*Posicion X e Y predeterminado.*/
/*    QSize dim(35,35);/*Dimensiones del circulo*/
/*    QRect rec(pos, dim);/*Posicion y dimension el circulo*/

/*    painter.drawEllipse(rec);/*Dibujar circulo*/
/*    QPoint pf(7, 7);
    QPoint medioArriba(pos);
    medioArriba.setX(medioArriba.x() + dim.height()/2);/*Punto medio de arriba*/
/*    QPoint medioAbajo(medioArriba);
    medioAbajo.setY(medioAbajo.y() + dim.height());/*Punto medio de abajo*/
/*    painter.drawLine(d, medioArriba);


    /*Creacion de X exclusiva*/
/*    QPoint p6(7, 7);
/*    QPoint p5(pos);
    /*Punto p1*/
/*    QPoint p1(p5.operator +=(p6));
    /*Punto p2*/
/*    QPoint p2(p1);
    p2.setX(p2.x()+20);
    /*Punto p3*/
/*    QPoint p3(p1);
/*    p3.setY(p3.y()+20);
    /*Punto p4*/
/*    QPoint p4(p2);
    p4.setY(p4.y()+20);


    QString tipoExclusivo("tipo:  ");
    QPoint puntoTipo(pos);
    puntoTipo.setX(puntoTipo.x() + dim.width());
    painter.drawText(puntoTipo, tipoExclusivo);
    painter.drawLine(p1, p4);/*Dibuja linea 1*/
/**   painter.drawLine(p2, p3);/*Dibuja linea 2*/


void HierarchyDialog::on_modeComboBox_currentIndexChanged(int index)
{
    this->mode = Mode(index);

    ui->lineEdit->setDisabled(true);
    if (mode == HierarchyDialog::Create){
        ui->listSub->clear();
        ui->listEnt->clear();
        ui->sup->clear();
        foreach(QGraphicsItem * item, sceneItems){
            if (item->type() == EntityItem::Type){
                ui->sup->addItem(((IGraphicItem*)item)->getName());
                //QString name = ((IGraphicItem*)item)->getName();
                //ui->listEnt->insertItem(0, name);
            }
        }
    }
    else
    if (mode == HierarchyDialog::Modify){
        //ui->rtypeLabel->setText("Relationship");
        ui->sup->clear();
        ui->listEnt->clear();

        foreach(QGraphicsItem * item, this->sceneItems){
            if (item->type() == HierarchyItem::Type)
                ui->sup->addItem(((IGraphicItem*)item)->getName());
        }
        /*if (ui->sup->count() > 0)
            on_rtypeComboBox_currentIndexChanged(0);  */  //so that nameLineEdit starts with rship text.
    }

}

void HierarchyDialog::on_sup_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    switch(mode){
        case(HierarchyDialog::Create):{
            ui->listEnt->clear();
            foreach(QGraphicsItem * item, sceneItems){
                if (item->type() == EntityItem::Type){
                    if(((IGraphicItem*)item)->getName() != ui->sup->currentText()){
                        //ui->sup->addItem(((IGraphicItem*)item)->getName());
                        QString name = ((IGraphicItem*)item)->getName();
                        ui->listEnt->insertItem(0, name);

                    }
                }
            }
            /*updateEntities(index);
            on_max1LineEdit_editingFinished();*/
            break;
        }
        case(HierarchyDialog::Modify):{
            HierarchyItem * hierarchy = getHierarchy();
            QList<EntityItem*> ents = hierarchy->getEntities();
            ui->listSub->clear();
            foreach(EntityItem* item, ents){
                if(item->getName() != ents.at(0)->getName())
                    ui->listSub->insertItem(0, item->getName());
            }
            foreach(QGraphicsItem * item, sceneItems){
                if (item->type() == EntityItem::Type){
                    bool exists= false;
                    foreach(EntityItem* item2, ents){
                        if(((IGraphicItem*)item)->getName() == item2->getName())
                            exists = true;
                    }
                    if(!exists)
                        ui->listEnt->insertItem(0, ((IGraphicItem*)item)->getName());
                }
            }

            on_exclusiveBool_clicked(hierarchy->getExclusive());
            //if(hierarchy->getExclusive())
            if(hierarchy->getTotal())
                ui->totalBool->setCheckState(Qt::Checked);

            /*ui->e1ComboBox->setEnabled(false);
            ui->e2ComboBox->setEnabled(false);
            ui->e3ComboBox->setEnabled(false);

            updateEntities(ents.size()-1);
            if (ents.size() == 1){
                ((QLineEdit*)entitiesUi.at(1)->itemAt(2)->widget())->setText(QString::number(rship->getCardinalities().at(1)->getMin()));
                ((QLineEdit*)entitiesUi.at(1)->itemAt(3)->widget())->setText(rship->getCardinalities().at(1)->getMax());
            }
            updateWeakEntUI();*/
            break;
        }
    }
}

void HierarchyDialog::on_lineEdit_editingFinished()
{
    if (mode == HierarchyDialog::Modify){
        HierarchyItem * hierarchy = getHierarchy();
        if (hierarchy != NULL){
            QString newName = ui->lineEdit->text();
            if (newName.isEmpty()){
                error->setError(Error::EmptyTypeName);
                this->show();
            }
            else{
                hierarchy->setTypeName(newName);
                ((MainWindow*)this->parent())->setSaved(false);
            }
        }
    }
}

bool HierarchyDialog::existsSupertype()
{
    /*Entity * father = NULL;
    foreach(QGraphicsItem * item, sceneItems){
        if (item->type() == EntityItem::Type){
            if(((IGraphicItem*)item)->getName() == ui->sup->currentText())
                father = ((EntityItem*)((IGraphicItem*)item)->getERItem())->getFather();
        }
    }
    while(father != NULL){
        for(int i = 0; i < ui->listSub->count(); i++){
            if(ui->listSub->item(i)->text() == father->getName())
                return true;
        }
        father = father->getFather();
    }*/
    return false;
}
