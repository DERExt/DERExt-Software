#include "associationdialog.h"
#include "ui_associationdialog.h"


#include "associationdialog.h"
#include "ui_associationdialog.h"
#include "rternaryitem.h"
#include "entityitem.h"


associationdialog::associationdialog(QList<QGraphicsItem*> items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::associationdialog)
{
    error = new Error(this);
    sceneItems.append(items);
    ui->setupUi(this);
    ui->lineEdit->setDisabled(true);

    foreach(QGraphicsItem * item, sceneItems){
        backup.append(((IGraphicItem*)item)->getCopy());
    }

    /*crear o modificar*/
    ui->modeComboBox->addItem("Create");
    ui->modeComboBox->addItem("Modify");
    this->mode = Mode(ui->modeComboBox->currentIndex());
}

associationdialog::~associationdialog()
{
    delete ui;
}




void associationdialog::on_buttonBox_accepted() {
    /*if(mode == associationdialog::Modify){
        auxModify = getHierarchy()->getEntities().at(0);
            removeHierarchy(getHierarchy()->getEntities());//Elimina la jerarquia de cada entidad
            MainWindow * window = (MainWindow*)this->parentWidget();
            window->deleteItem(getHierarchy());
            sceneItems.removeOne(getHierarchy());
        }*/
    MainWindow * window = (MainWindow*)this->parentWidget();
    EntityItem * item = createAssociation();
    window->addItem(item);
    error->setError(Error::NoError);
    this->close();
}
/*
void associationdialog::removeRelationship(QList<RelationshipItem*> list){
    int cont = list.count();
    for(int i = 1; i< cont; i++){
        ((Entity*)list.at(i)->getERItem())->RemoveHierarchySon();
    }
}

/*
void associationdialog::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, sceneItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup)
        main->addItem((IGraphicItem*)item);
    this->close();
}
*/
EntityItem *associationdialog::createAssociation() {
    RelationshipItem * relationship;

    foreach(QGraphicsItem * item, sceneItems){
        if (ui->listSub->count() > 0 && item->type() == RelationshipItem::Type && ui->listSub->item(0)->text() == ((IGraphicItem*)item)->getName()){
            relationship = ((RelationshipItem*)item);
            break;
        }
    }

    MainWindow * window = (MainWindow*)this->parentWidget();
    return new EntityItem(this->ui->lineEdit->text(), NULL, NULL, NULL, window, false, false, false, NULL, true, relationship);
}

/*
HierarchyItem *associationdialog::getHierarchy()
{
    if (mode == associationdialog::Modify){
        QString currentName = ui->sup->currentText();
        if (currentName.isEmpty()) return NULL;

        foreach(QGraphicsItem * item, sceneItems){
            if (((IGraphicItem*)item)->getName() == currentName)
                return (HierarchyItem*)item;
        }
    }
}
*/
void associationdialog::on_modeComboBox_currentIndexChanged(int index) {
    this->mode = Mode(index);
    if (mode == associationdialog::Create){
        ui->lineEdit->setDisabled(false);
        ui->listSub->clear();
        ui->listEnt->clear();
        foreach(QGraphicsItem * item, sceneItems){
            if (item->type() == RelationshipItem::Type){
                QString name = ((IGraphicItem*)item)->getName();
                ui->listEnt->insertItem(0, name);
            }
        }
    }
    else
    if (mode == associationdialog::Modify){
        ui->listEnt->clear();
        ui->lineEdit->setDisabled(true);
    }

}
/*

void associationdialog::on_lineEdit_editingFinished()
{
    if (mode == associationdialog::Modify){
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
*/

void associationdialog::on_pushAdd_clicked() {

    QString current = (ui->listSub->count() > 0) ? ui->listSub->item(0)->text() : NULL;

    if(ui->listEnt->currentItem() != NULL){
        QString name = ui->listEnt->currentItem()->text();
        ui->listSub->clear();
        ui->listSub->insertItem(0, name);
        delete ui->listEnt->currentItem();
        if (current != NULL)
            ui->listEnt->addItem(current);
    }
}
