#include "hierarchydialog.h"
#include "ui_hierarchydialog.h"
#include "rternaryitem.h"
#include "entityitem.h"

HierarchyDialog::HierarchyDialog(QList<QGraphicsItem*> items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HierarchyDialog)
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
            auxModify = getHierarchy()->getEntities().at(0);/***/
            removeHierarchy(getHierarchy()->getEntities());/*Elimina la jerarquia de cada entidad*/
            MainWindow * window = (MainWindow*)this->parentWidget();/***/
            window->deleteItem(getHierarchy());/*******************/
            sceneItems.removeOne(getHierarchy());/*******************/
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

void HierarchyDialog::removeHierarchy(QList<EntityItem*> list){
    int cont = list.count();
    for(int i = 1; i< cont; i++){
        ((Entity*)list.at(i)->getERItem())->RemoveHierarchySon();
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
        }
    }
    foreach(QGraphicsItem * item, sceneItems){
        if (item->type() == EntityItem::Type){
            for(int i = 0; i < longList ; i++){
                if(ui->listSub->item(i)->text() == ((IGraphicItem*)item)->getName()){
                    entities << (((EntityItem*)item));
                }
            }
        }
    }
        bool exc = ui->exclusiveBool->checkState();
        bool tot = ui->totalBool->checkState();
        MainWindow * window = (MainWindow*)this->parentWidget();
        return new HierarchyItem(NULL,error,window, ui->lineEdit->text(), exc ,tot, entities);

}

bool HierarchyDialog::isFatherValid(EntityItem* item)
{/************************************************/
    Entity *ent = ((Entity*)item->getERItem());
    if(ent->getHierarchy() == NULL)
        return true;
    return false;
}
bool HierarchyDialog::isSonValid(EntityItem* item)
{/************************************************/
    Entity *ent = ((Entity*)item->getERItem());
    if(ent->getHierarchySon() == NULL && ent->isSubtype())
        return true;
    return false;
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

void HierarchyDialog::on_modeComboBox_currentIndexChanged(int index)
{
    this->mode = Mode(index);

    ui->lineEdit->setDisabled(true);

    QList<QString> names;

    if (mode == HierarchyDialog::Create){
        ui->listSub->clear();
        ui->listEnt->clear();
        ui->sup->clear();
        foreach(QGraphicsItem * item, sceneItems){
            if (item->type() == EntityItem::Type && isFatherValid((EntityItem*)item)){
                names << ((IGraphicItem*)item)->getName();
            }
        }
    }
    else
    if (mode == HierarchyDialog::Modify){
        ui->sup->clear();
        ui->listEnt->clear();

        foreach(QGraphicsItem * item, this->sceneItems){
            if (item->type() == HierarchyItem::Type)
                names << ((IGraphicItem*)item)->getName();
        }
    }

    std::sort(names.begin(),names.end());
    ui->sup->addItems(names);
}

void HierarchyDialog::on_sup_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    QList<QString> names;

    switch(mode){
        case(HierarchyDialog::Create):{
            ui->listEnt->clear();
            foreach(QGraphicsItem * item, sceneItems){
                if (item->type() == EntityItem::Type){
                    if(((IGraphicItem*)item)->getName() != ui->sup->currentText()){
                        if(isSonValid((EntityItem*)item)){
                            QString name = ((IGraphicItem*)item)->getName();
                            names << name;
                        }
                    }
                }
            }
            std::sort(names.begin(), names.end());
            ui->listEnt->insertItems(0,names);
            break;
        }
        case(HierarchyDialog::Modify):{
            HierarchyItem * hierarchy = getHierarchy();
            QList<EntityItem*> ents = hierarchy->getEntities();
            ui->listSub->clear();
            ui->listEnt->clear();


            foreach(EntityItem* item, ents){
                if(item->getName() != ents.at(0)->getName())
                    names << item->getName();
            }

            std::sort(names.begin(), names.end());
            ui->listSub->insertItems(0, names);
            names.clear();

            foreach(QGraphicsItem * item, sceneItems){
                if(item->type() == EntityItem::Type && isSonValid((EntityItem*)item)){
                    names << ((IGraphicItem*)item)->getName();
                }
            }

            std::sort(names.begin(), names.end());
            ui->listEnt->insertItems(0, names);
            names.clear();

            on_exclusiveBool_clicked(hierarchy->getExclusive());
            if(hierarchy->getTotal())
                ui->totalBool->setCheckState(Qt::Checked);
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
