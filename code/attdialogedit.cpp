#include "attdialogedit.h"
#include "mainwindow.h"
#include "weakentity.h"
#include "ui_attdialogedit.h"

AttDialogEdit::AttDialogEdit(QList<QGraphicsItem*> listItems, QWidget *parent) :QDialog(parent), ui(new Ui::AttDialogEdit)
{
    ui->setupUi(this);

    QStringList tableHeader;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(3);
    tableHeader<<"Name"<<"Optional"<< "Multivalued";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(true);    
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->error = new Error(parent);
    this->listItems = listItems;
    if ((listItems.first())->type() == EntityItem::Type){
        this->item = (IGraphicItem*)(listItems.first());
        this->setAttributes(item->getAllAttributes());
    }
    foreach(QGraphicsItem* item, listItems){
        if (item->type() == EntityItem::Type)
            this->ui->itemComboBox->addItem(((IGraphicItem*)item)->getName());
        this->backup.append(((IGraphicItem*)item)->getCopy());
    }
    this->setWindowTitle("Edit Key");
}

AttDialogEdit::~AttDialogEdit()
{
    delete ui;
}

void AttDialogEdit::setAttributes(QList<Attribute*> atts)
{
    names.clear();
    foreach(Attribute *att, atts){
        names.push_back(att->getName());
        att->addToTable(this->ui->tableWidget);
    }    
    Entity *ent = (Entity*)((EntityItem*)item)->getERItem();

    Attribute *pk =ent->getPrimaryKey();
    if(pk!=NULL){
        QTableWidgetItem *tmp = ui->tableWidget->item(0,0);
        if(tmp!=NULL && tmp->text() == pk->getName())
            tmp->setIcon(QIcon(":/img/Pk.png"));
    }

    int totalRows = ui->tableWidget->rowCount();
    QList<Attribute *> sks = ent->getSecondaryKeys();
    QList<QString> aux;
    foreach(Attribute *att, sks)
        aux.append(att->getName());
    for(int i = 0 ; i<totalRows ; i++){
        if(aux.contains(ui->tableWidget->item(i, 0)->text()))
            ui->tableWidget->item(i, 0)->setIcon(QIcon(":/img/Sk.png"));
    }
}

Attribute *AttDialogEdit::getAttribute(int row)
{
    QString name = names.at(row);
    foreach(Attribute *att, item->getAllAttributes()){
        if(att->getName() == name)
            return att;
    }
    return NULL;
}

void AttDialogEdit::on_primaryKeyPButton_clicked()
{
    Entity *ent = (Entity*)((EntityItem*)item)->getERItem();

    bool changes=false;
    QList<QTableWidgetItem *> tmp = ui->tableWidget->selectedItems();
    if(!tmp.isEmpty()){
        Attribute *exPk = ent->getPrimaryKey();
        if(ent->setPrimaryKey(getAttribute(tmp.first()->row())))
            changes = true;
        else error->setError(Error::InvalidPK);
        if(changes){
            if(exPk!=NULL){
                int totalRows = ui->tableWidget->rowCount();
                for(int i = 0 ; i<totalRows ; i++)
                    if(ui->tableWidget->item(i, 0)->text() == exPk->getName())
                        ui->tableWidget->item(i, 0)->setIcon(QIcon(""));
            }
            tmp.first()->setIcon(QIcon(":/img/Pk.png"));
            ((GraphicView*)(item->scene()->parent()))->setSaved(false);
        }
    }

}

void AttDialogEdit::on_secondaryKeyPButton_clicked()
{
    Entity *ent = (Entity*)((EntityItem*)item)->getERItem();

    QList<QTableWidgetItem *> tmp = ui->tableWidget->selectedItems();

    if(!tmp.isEmpty()){
        if(tmp.size()== 1){
            if(ent->setSecondaryKey(getAttribute(tmp.first()->row()))){
                tmp.first()->setIcon(QIcon(":/img/Sk.png"));
                ((GraphicView*)(item->scene()->parent()))->setSaved(false);
            }
        }
        else{
            QList<Attribute *> aux;
            foreach(QTableWidgetItem *item, tmp)
                aux.append(getAttribute(item->row()));
            if(ent->setSecondaryKeyComp(aux)){
                foreach(QTableWidgetItem *item, tmp)
                    item->setIcon(QIcon(":/img/Sk.png"));
            }

        }
    }
}

void AttDialogEdit::on_eraseKeyPButton_clicked()
{
    Entity *ent = (Entity*)((EntityItem*)item)->getERItem();

    QList<QTableWidgetItem *> tmp = ui->tableWidget->selectedItems();

    if(!tmp.isEmpty()){
        if(ent->removeSecondaryKey(tmp.first()->text())){
            tmp.first()->setIcon(QIcon(""));
            ((GraphicView*)(item->scene()->parent()))->setSaved(false);
        }
    }
}

void AttDialogEdit::on_itemComboBox_currentIndexChanged(const QString &itemName)
{
    bool found = false;
    QList<QGraphicsItem*>::iterator it = listItems.begin();
    while(it!=listItems.end() && !found){
        if(((IGraphicItem*)(*it))->getName() == itemName){
            this->item = (IGraphicItem*)(*it);
            found = true;
        }
        else it++;
    }
    ui->itemLabel->setText("Item - "+item->getType());
    ui->tableWidget->clear();
    QStringList tableHeader;
    tableHeader<<"Name"<<"Optional"<< "Multivalued";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->setRowCount(0);
    this->setAttributes(item->getAllAttributes());
}

void AttDialogEdit::on_buttonBox_accepted()
{
    this->close();
}

void AttDialogEdit::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, listItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup){
        main->addItem((IGraphicItem*)item);
    }
    this->close();
}
