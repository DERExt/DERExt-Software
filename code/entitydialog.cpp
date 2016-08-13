#include "entitydialog.h"
#include "ui_entitydialog.h"

entityDialog::entityDialog(QList<QGraphicsItem*> items,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::entityDialog)
{
    ui->setupUi(this);
    ui->entitiesComboBox->setEnabled(false);
    ui->checkBox->setChecked(false);
    this->listItems.append(items);
    ui->nameLineEdit->setFocus();

    foreach(QGraphicsItem * item, listItems){
        if (item->type() == EntityItem::Type){
            ui->entitiesComboBox->addItem(((IGraphicItem*)item)->getName());            
        }
        backup.append(((IGraphicItem*)item)->getCopy());
    }
    error = new Error(this);

    ui->modeComboBox->addItem("Create");
    ui->modeComboBox->addItem("Modify");
    mode = entityDialog::Create;
}

entityDialog::~entityDialog()
{
    delete ui;
}

void entityDialog::on_buttonBox_accepted()
{
    if (mode == entityDialog::Create){
        MainWindow * window = (MainWindow*)this->parentWidget();

        QString entityName = ui->nameLineEdit->text();

        if(!entityName.isEmpty()){
            QList<QString> names;
            foreach(QGraphicsItem * item, listItems)
                names.append(((IGraphicItem*)item)->getName());
            if(!names.contains(entityName)){
                EntityItem * entity;
                if (ui->checkBox->isChecked() && !listItems.isEmpty()){         //si es entidad debil
                    EntityItem * strongEnt = getCurrentEntity();
                    entity = new EntityItem(entityName,NULL,NULL,error,window,true,(Entity*)strongEnt->getERItem());
                    window->addItem(entity);        //necesito la entidad en la scene antes de poder agregar Rship
                    QList<EntityItem*> entities;
                    entities << strongEnt << entity;
                    QList<Cardinality*> cards;
                    cards << new Cardinality(1,"1") << new Cardinality(1,"N");
                    QString name("depends"+QString::number(this->rshipCount("depends")));
                    RBinaryItem * rship = new RBinaryItem(name,NULL,NULL,error,window,entities,cards,true,false);
                    window->addItem(rship);

                    //allow to edit the new relationship
                    listItems << rship;
                    RelationshipDialog * rshipDialog = new RelationshipDialog(listItems,this->parentWidget());
                    rshipDialog->setRelationship(name);
                    rshipDialog->show();
                }
                else{
                    EntityItem * entity = new EntityItem(entityName, NULL, NULL, error, window);
                    window->addItem(entity);
                }
                error->setError(Error::NoError);
            }
            else {
                error->setError(Error::DuplicateEntity);
                this->show();
            }
        }
        else{
            error->setError(Error::EmptyNameEntity);
            this->show();
        }
    }
}

void entityDialog::on_checkBox_stateChanged(int state)
{
    if (state == 2)
        ui->entitiesComboBox->setEnabled(true);
    else
        ui->entitiesComboBox->setEnabled(false);
}

EntityItem* entityDialog::getCurrentEntity(){
    foreach(QGraphicsItem * item, listItems){
        if (((IGraphicItem*)item)->getName() == ui->entitiesComboBox->currentText())
                return (EntityItem*)item;
    }
    return NULL;
}

int entityDialog::rshipCount(QString rshipName)
{
    int count = 0;
    foreach(QGraphicsItem * item, listItems){
        QString name = ((IGraphicItem*)item)->getName();
        if (name.contains(rshipName))
            count++;
    }
    return count;
}

void entityDialog::on_modeComboBox_currentIndexChanged(int index)
{
    mode = entityDialog::Mode(index);

    switch(mode){
        case(entityDialog::Create):{
            ui->checkBox->setHidden(false);
            ui->entitiesComboBox->setEnabled(false);
            ui->comboBoxLayout->setContentsMargins(0,0,0,0);
            ui->gridLayout->removeItem(ui->comboBoxLayout);
            ui->gridLayout->removeItem(ui->lineEditLayout);
            ui->gridLayout->addItem(ui->lineEditLayout,1,0);
            ui->gridLayout->addItem(ui->comboBoxLayout,2,0);
            ui->nameLineEdit->setText("");
            break;
        }
        case(entityDialog::Modify):{
            ui->checkBox->setHidden(true);
            ui->comboBoxLayout->setContentsMargins(0,15,0,0);
            ui->entitiesComboBox->setEnabled(true);
            ui->gridLayout->removeItem(ui->comboBoxLayout);
            ui->gridLayout->removeItem(ui->lineEditLayout);
            ui->gridLayout->addItem(ui->comboBoxLayout,1,0);
            ui->gridLayout->addItem(ui->lineEditLayout,2,0);
            on_entitiesComboBox_currentIndexChanged(0);
            break;
        }
    }
}

void entityDialog::on_nameLineEdit_editingFinished()
{
    if (mode == entityDialog::Modify){
        QString newName = ui->nameLineEdit->text();
        if (newName.isEmpty()){
            error->setError(Error::EmptyNameEntity);
            this->show();
        }
        else{
            QList<QString> names;
            foreach(QGraphicsItem * item, listItems)
                names.append(((IGraphicItem*)item)->getName());
            if(!names.contains(newName)){
                EntityItem * ent = getCurrentEntity();
                ent->setName(newName);
                ((MainWindow*)this->parent())->setSaved(false);
            }
            else{
                error->setError(Error::DuplicateEntity);
                this->show();
            }
        }
    }
}

void entityDialog::on_entitiesComboBox_currentIndexChanged(int index)
{
    if (mode == entityDialog::Modify && index >= 0){
        ui->nameLineEdit->setText(ui->entitiesComboBox->currentText());
    }
}

void entityDialog::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, listItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup)
        main->addItem((IGraphicItem*)item);
    this->close();
}
