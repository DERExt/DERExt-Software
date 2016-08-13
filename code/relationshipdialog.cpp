#include "relationshipdialog.h"
#include "ui_relationshipdialog.h"
RelationshipDialog::RelationshipDialog(QList<QGraphicsItem*> items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RelationshipDialog)
{
    ui->setupUi(this);
    error = new Error(this);
    sceneItems.append(items);

    //listado para manejar las layout de entidad
    entitiesUi.append(ui->e1Layout);
    entitiesUi.append(ui->e2Layout);
    entitiesUi.append(ui->e3Layout);

    //cargar las entidades
    foreach(QGraphicsItem * item,items){
        if (item->type() == EntityItem::Type){
            ui->e1ComboBox->addItem(((IGraphicItem*)item)->getName());
            ui->e2ComboBox->addItem(((IGraphicItem*)item)->getName());
            ui->e3ComboBox->addItem(((IGraphicItem*)item)->getName());
        }
        backup.append(((IGraphicItem*)item)->getCopy());
    }

    //modos
    ui->modeComboBox->addItem("Create");
    ui->modeComboBox->addItem("Modify");
    this->mode = Mode(ui->modeComboBox->currentIndex());

    //deshabilitar los correspondientes botones
    ui->weakRadioButton->setEnabled(false);
    ui->weakRadioButton2->setEnabled(false);
}

RelationshipDialog::~RelationshipDialog()
{
    delete ui;
}

/**
 * @brief RelationshipDialog::setRelationship
 * Sets the Dialog in Modify mode,
 * allowing to edit the rship with name rshipName.
 */
void RelationshipDialog::setRelationship(QString rshipName)
{
    this->mode = RelationshipDialog::Modify;
    ui->modeComboBox->setCurrentIndex(1);
    int index = ui->rtypeComboBox->findText(rshipName,Qt::MatchExactly);
    if (index != -1)
        ui->rtypeComboBox->setCurrentIndex(index);
    }

void RelationshipDialog::enableEntity(int index, bool enable){
    if (index<0 || index>=entitiesUi.size())
        return;

    QVBoxLayout * layout = entitiesUi.at(index);

    for (int i = 1; i < 4 ; i++)
        layout->itemAt(i)->widget()->setEnabled(enable);

    QComboBox * box = (QComboBox*)layout->itemAt(1)->widget();

    if (!enable && box != NULL){
        box->addItem("");
        box->setCurrentIndex(box->count()-1);
    }
    else {
        if (box->itemText(box->count()-1) == ""){
            box->removeItem(box->count()-1);
        }
    }
}


/**
 * Construir la relacion a partir de los datos ingresados
 */
RelationshipItem * RelationshipDialog::createRelationship(){
    MainWindow * window = (MainWindow*)this->parentWidget();
    int index = ui->rtypeComboBox->currentIndex();
    QList<EntityItem*> entities;
    QList<Cardinality*> cards;

    //prepare for whatever kind of rship
    cards << rightCardinalities(0);
    cards << rightCardinalities(1);

    if (cards.at(0)->getMin()==-1 || cards.at(1)->getMin()==-1){
        error->setError(Error::Cardinality);
        this->show();
        return NULL;
    }
    entities << getEntity(0);
    if (index == 0){
        RUnaryItem * rship = new RUnaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window,entities,cards);

        return rship;
    }

    //at least binary relationship
    entities << getEntity(1);

    if (!differentEntities(entities) && index==1){
        QMessageBox::warning(this->parentWidget(),"Repeated entities",
                             "Entities are repeated in the relationship.");
        entities.pop_back();
        return new RUnaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window,entities,cards);
    }
    if (error->getError()==Error::NoError && index == 1){
        if (ui->weakRadioButton->isEnabled() && ui->weakRadioButton->isChecked()){
            if (entities.at(0)->getHasIdDependency()){
                QMessageBox::warning(this->parentWidget(),"Weak entity","A weak entity cannot have many regular entities.");
                return NULL;
            }
            return new RBinaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window, entities, cards, true, true);
        } else
            if (ui->weakRadioButton2->isEnabled() && ui->weakRadioButton2->isChecked()){
                if (entities.at(1)->getHasIdDependency()){
                    QMessageBox::warning(this->parentWidget(),"Weak entity","A weak entity cannot have many regular entities.");
                    return NULL;
                }
                return new RBinaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window, entities, cards, true, false);
            } else return new RBinaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window, entities, cards);
    }

    //has to be trinary or higher
    entities << getEntity(2);
    cards << rightCardinalities(2);

    if (cards.at(0)->getMin()==-1 || cards.at(1)->getMin()==-1 || cards.at(2)->getMin()==-1){
        error->setError(Error::Cardinality);
        this->show();
        return NULL;
    }
    if (!differentEntities(entities)){
        QMessageBox::warning(this->parentWidget(),"Repeated entities",
                             "Entities are repeated in the relationship.");
    }
    if (index == 2) return new RTernaryItem(ui->nameLineEdit->text(),NULL,NULL,error,window,entities,cards);
    return NULL;
}

/**
 * Obtener EntityItem a partir de los datos de una
 * de las layouts de Entidad, dado por entityNumber
 */
EntityItem * RelationshipDialog::getEntity(int entityNumber){
    if (entityNumber<0 || entityNumber>=entitiesUi.size())
        return NULL;

    QComboBox * box = (QComboBox*)entitiesUi.at(entityNumber)->itemAt(1)->widget();
    QString currentName = box->currentText();

    foreach(QGraphicsItem * item, sceneItems){
        if (((EntityItem*)item)->getName() == currentName){
            if ((entityNumber == 0 && ui->weakRadioButton->isEnabled() && ui->weakRadioButton->isChecked())
              || ((entityNumber == 1) && ui->weakRadioButton2->isEnabled() && ui->weakRadioButton2->isChecked() )){
                ((EntityItem*)item)->setIsWeak(true, getEntity(abs(entityNumber -1)));
            }
            return (EntityItem*)item;
        }
    }
    return NULL;
}

RelationshipItem * RelationshipDialog::getRelationship(){
    if (mode == RelationshipDialog::Modify){
        QString currentName = ui->rtypeComboBox->currentText();
        if (currentName.isEmpty()) return NULL;

        foreach(QGraphicsItem * item, sceneItems){
            if (((IGraphicItem*)item)->getName() == currentName)
                return (RelationshipItem*)item;
        }
    }
    return NULL;
}

/**
 * @brief RelationshipDialog::updateIdDependency
 * Ante los cambios en cardinalidad, puede ser que una relacion
 * deje o pueda ser de IdDependency (entidad debil).
 * El método permite hacer los ajustes necesarios.
 */
void RelationshipDialog::updateIdDependency(bool idDep)
{
    RelationshipItem * rship = getRelationship();

    if (rship!=NULL && rship->getEntities().size()==2
            && mode == RelationshipDialog::Modify){
        if (idDep){
            if (ui->weakRadioButton->isEnabled() && ui->weakRadioButton->isChecked()){
                if (rship->getEntities().size()==2 && rship->getEntities().at(0)->getHasIdDependency())
                    QMessageBox::warning(this->parentWidget(),"Weak entity","A weak entity cannot have many regular entities.");
                else ((RBinaryItem*)rship)->setIdDependency(true,true);
            }
            if (ui->weakRadioButton2->isEnabled() && ui->weakRadioButton2->isChecked()){
                if (rship->getEntities().size()==2 && rship->getEntities().at(1)->getHasIdDependency())
                    QMessageBox::warning(this->parentWidget(),"Weak entity","A weak entity cannot have many regular entities.");
                else ((RBinaryItem*)rship)->setIdDependency(true,false);
            }
            ((MainWindow*)this->parent())->setSaved(false);
        }
        else{
            if (((RBinaryItem*)rship)->getIdDependency()){
                QMessageBox::warning(this->parentWidget(),"Identification dependency change",
                                     "The relationship no longer represents an identification dependency");
                ((RBinaryItem*)rship)->setIdDependency(false);
            }
        }
    }
}

/**
 * Construir cardinalidad (min,max) de entidad, obteniendo
 * los datos ingresados segun el numero pasado por param.
 */
Cardinality * RelationshipDialog::rightCardinalities(int entity){
    QString minCard = ((QLineEdit*)entitiesUi.at(entity)->itemAt(2)->widget())->text();
    QString maxCard = ((QLineEdit*)entitiesUi.at(entity)->itemAt(3)->widget())->text();

    if (!minCard.isEmpty() && !maxCard.isEmpty()){
        QString min = minCard;
        QString max = maxCard;

        maxCard.remove(QRegExp("[0-9]+"));
        minCard.remove(QRegExp("[0-9]+"));
        if (minCard.isEmpty()){              //min debe ser un numero
            if (maxCard.isEmpty()){          //max es un numero tmb
                if (min.toInt() <= max.toInt() && max.toInt()>0)
                    return new Cardinality(min.toInt(),max);
            }
            else{                           //max debe ser letra
                max.remove(QRegExp("[A-Z]"));
                if (max.isEmpty())
                    return new Cardinality(min.toInt(),maxCard);
            }
        }
    }
    return new Cardinality(-1,"");
}

/**
 * Revisar que las entidades en el listado sean
 * todas diferentes entre si.
 */
bool RelationshipDialog::differentEntities(QList<EntityItem*> list){
    foreach(EntityItem* item, list){
        QList<EntityItem*> items;
        items.append(list);
        items.removeOne(item);
        foreach(IGraphicItem * item2, items)
            if (item->getName()==item2->getName())
                return false;
    }
    return true;
}

void RelationshipDialog::updateWeakEntUI(){
    bool wasEnabled = (ui->weakRadioButton->isEnabled() || ui->weakRadioButton2->isEnabled());
    ui->weakRadioButton->setEnabled(false);
    ui->weakRadioButton2->setEnabled(false);

    if ((ui->rtypeComboBox->currentIndex() == 1 && mode == RelationshipDialog::Create)
        || (mode==RelationshipDialog::Modify && getRelationship()!=NULL && getRelationship()->getEntities().size()==2)){
        Cardinality * card1 = rightCardinalities(0);
        Cardinality * card2 = rightCardinalities(1);

        if (card1->maxIsMultiple()
                && card2->getMin()==1 && !card2->maxIsMultiple())           //cardinalidad N
            ui->weakRadioButton->setEnabled(true);
        else                                                                //cardinalidad 1
            if (!card1->maxIsMultiple() && card1->getMin()==1
                    && card2->maxIsMultiple())
                ui->weakRadioButton2->setEnabled(true);
    }

    if (!ui->weakRadioButton->isEnabled() && !ui->weakRadioButton2->isEnabled() && wasEnabled)
        this->updateIdDependency(false);
}

void RelationshipDialog::updateEntities(int limit)
{
    //habilitar todo ,deshaciendo previos cambios
    for (int i = 0; i < entitiesUi.size(); i++)
        enableEntity(i,true);

    //deshabilitar los correspondientes
    for (int j = entitiesUi.size()-1; j > limit; j--)
        enableEntity(j,false);

    if (limit == 0){
        entitiesUi.at(1)->itemAt(2)->widget()->setEnabled(true);
        entitiesUi.at(1)->itemAt(3)->widget()->setEnabled(true);
    }
}

void RelationshipDialog::updateRshipCardinality(int index, Cardinality * newCard){
    if (mode == RelationshipDialog::Modify){
        RelationshipItem * rship = getRelationship();
        if (rship != NULL){
            if (index < 0 || index >= rship->getCardinalities().size()) return;
            Cardinality * card = rship->getCardinalities().at(index);

            if (newCard->getMin() == -1){
                error->setError(Error::Cardinality);
                this->show();
            }
            else{
                card->setMin(newCard->getMin());
                card->setMax(newCard->getMax());
                ((MainWindow*)this->parent())->setSaved(false);
            }
        }
    }
}

void RelationshipDialog::on_min1LineEdit_editingFinished()
{
    updateWeakEntUI();
    updateRshipCardinality(0,this->rightCardinalities(0));
}

void RelationshipDialog::on_max1LineEdit_editingFinished()
{
    updateWeakEntUI();
    updateRshipCardinality(0,this->rightCardinalities(0));
}

void RelationshipDialog::on_min2LineEdit_editingFinished()
{
    updateWeakEntUI();
    updateRshipCardinality(1,this->rightCardinalities(1));
}

void RelationshipDialog::on_max2LineEdit_editingFinished()
{
    updateWeakEntUI();
    updateRshipCardinality(1,this->rightCardinalities(1));
}

void RelationshipDialog::on_min3LineEdit_editingFinished()
{
    updateRshipCardinality(2,this->rightCardinalities(2));
}

void RelationshipDialog::on_max3LineEdit_editingFinished()
{
    updateRshipCardinality(2,this->rightCardinalities(2));
}

/**
 * Ante el cambio de tipo de relacion,
 * deshabilitar la entrada de datos de todas las entidades que no correspondan
 */
void RelationshipDialog::on_rtypeComboBox_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    switch(mode){
        case(RelationshipDialog::Create):{
            updateEntities(index);
            on_max1LineEdit_editingFinished();
            break;
        }
        case(RelationshipDialog::Modify):{
            ui->nameLineEdit->setText(ui->rtypeComboBox->itemText(index));
            RelationshipItem * rship = getRelationship();
            QList<EntityItem*> ents = rship->getEntities();

            for (int i = 0; i< ents.size(); i++){
                QVBoxLayout * layout = entitiesUi.at(i);
                ((QComboBox*)layout->itemAt(1)->widget())->clear();
                ((QComboBox*)layout->itemAt(1)->widget())->addItem(ents.at(i)->getName());
                ((QLineEdit*)layout->itemAt(2)->widget())->setText(QString::number(rship->getCardinalities().at(i)->getMin()));
                ((QLineEdit*)layout->itemAt(3)->widget())->setText(rship->getCardinalities().at(i)->getMax());
            }

            ui->e1ComboBox->setEnabled(false);
            ui->e2ComboBox->setEnabled(false);
            ui->e3ComboBox->setEnabled(false);

            updateEntities(ents.size()-1);
            if (ents.size() == 1){
                ((QLineEdit*)entitiesUi.at(1)->itemAt(2)->widget())->setText(QString::number(rship->getCardinalities().at(1)->getMin()));
                ((QLineEdit*)entitiesUi.at(1)->itemAt(3)->widget())->setText(rship->getCardinalities().at(1)->getMax());
            }
            updateWeakEntUI();
            break;
        }
    }
}

void RelationshipDialog::on_modeComboBox_currentIndexChanged(int index)
{
    this->mode = Mode(index);

    //enable everything by default (create mode)
    ui->e1ComboBox->setEnabled(true);
    ui->e2ComboBox->setEnabled(true);
    ui->e3ComboBox->setEnabled(true);
    ui->e1ComboBox->clear();
    ui->e2ComboBox->clear();
    ui->e3ComboBox->clear();
    ui->nameLineEdit->clear();
    ui->rtypeLabel->setText("Relationship type");
    disconnect(ui->rtypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_rtypeComboBox_currentIndexChanged(int)));
    ui->rtypeComboBox->clear();
    ui->rtypeComboBox->addItem("Unary");
    ui->rtypeComboBox->addItem("Binary");
    ui->rtypeComboBox->addItem("Ternary");
    foreach(QGraphicsItem * item, sceneItems){
        if (item->type() == EntityItem::Type){
            ui->e1ComboBox->addItem(((IGraphicItem*)item)->getName());
            ui->e2ComboBox->addItem(((IGraphicItem*)item)->getName());
            ui->e3ComboBox->addItem(((IGraphicItem*)item)->getName());
        }
    }

    if (mode == RelationshipDialog::Modify){
        ui->rtypeLabel->setText("Relationship");
        ui->rtypeComboBox->clear();
        foreach(QGraphicsItem * item, this->sceneItems){
            if (item->type() == RelationshipItem::Type)
                ui->rtypeComboBox->addItem(((IGraphicItem*)item)->getName());
        }
        if (ui->rtypeComboBox->count() > 0)
            on_rtypeComboBox_currentIndexChanged(0);    //so that nameLineEdit starts with rship text.
    }

    //disconnect and connect necesary to rtypeComboBox to work properly
    connect(ui->rtypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_rtypeComboBox_currentIndexChanged(int)));
    if (mode == RelationshipDialog::Create)
        ui->rtypeComboBox->setCurrentIndex(1);
}

void RelationshipDialog::on_buttonBox_accepted()
{
    if (mode == RelationshipDialog::Create){
        if (ui->nameLineEdit->text().isEmpty()){
            error->setError(Error::EmptyNameRship);
            this->show();
        }
        else{
            //revisar que no se encuentre repetido
            QList<QString> names;
            foreach(QGraphicsItem * item, sceneItems)
                names.append(((IGraphicItem*)item)->getName());

            RelationshipItem * item = createRelationship();
            if (item){
                if (names.contains(item->getName())){
                    error->setError(Error::NameTaken);
                    this->show();
                }
                else {

                    MainWindow * window = (MainWindow*)this->parentWidget();
                    window->addItem(item);
                }
            }
            error->setError(Error::NoError);
        }
    }
}

void RelationshipDialog::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, sceneItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup)
        main->addItem((IGraphicItem*)item);
    this->close();
}

void RelationshipDialog::on_nameLineEdit_editingFinished()
{
    if (mode == RelationshipDialog::Modify){
        RelationshipItem * rship = getRelationship();
        if (rship != NULL){
            QString newName = ui->nameLineEdit->text();
            QList<QString> names;
            foreach(QGraphicsItem * item, sceneItems)
                names.append(((IGraphicItem*)item)->getName());
            names.removeOne(rship->getName());

            if (names.contains(newName)){
                    error->setError(Error::NameTaken);
                    this->show();
            }
            else
                if (newName.isEmpty()){
                    error->setError(Error::EmptyNameRship);
                    this->show();
                }
                else
                    if (rship != NULL){
                        rship->setName(newName);
                        ((MainWindow*)this->parent())->setSaved(false);
                        //reload rships with new names
                        ui->rtypeComboBox->clear();
                        foreach(QGraphicsItem * item, this->sceneItems){
                            if (item->type() == RelationshipItem::Type)
                                ui->rtypeComboBox->addItem(((IGraphicItem*)item)->getName());
                        }
                    }
        }
    }
}

void RelationshipDialog::on_weakRadioButton_clicked()
{
    this->updateIdDependency(ui->weakRadioButton->isChecked());
}

void RelationshipDialog::on_weakRadioButton2_clicked()
{
    this->updateIdDependency(ui->weakRadioButton2->isChecked());
}

void RelationshipDialog::on_rtypeComboBox_activated(const QString &arg1)
{

}

void RelationshipDialog::on_e1ComboBox_activated(const QString &arg1)
{

}
