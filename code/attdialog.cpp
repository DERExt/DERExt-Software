#include "attdialog.h"
#include "ui_attdialog.h"
#include "attdialogadd.h"
#include "mainwindow.h"
#include <QHeaderView>
#include <QDebug>

AttDialog::AttDialog(QList<QGraphicsItem*> listItems, QWidget *parent) :QDialog(parent), ui(new Ui::AttDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Attributes Managment");
    this->listItems = listItems;
    this->error = new Error(this);
    foreach(QGraphicsItem* item, listItems){
        if(((IGraphicItem*)item)->type() != HierarchyItem::Type)
            this->ui->CBoxItems->addItem(((IGraphicItem*)item)->getName());
        this->backup.append(((IGraphicItem*)item)->getCopy());
    }
    QStringList tableHeader;
    ui->TableAtts->setRowCount(0);
    ui->TableAtts->setColumnCount(3);
    tableHeader<<"Name"<<"Optional"<< "Multivalued";
    ui->TableAtts->setHorizontalHeaderLabels(tableHeader);
    ui->TableAtts->verticalHeader()->setVisible(false);
    ui->TableAtts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TableAtts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TableAtts->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->TableAtts->setShowGrid(true);
    ui->TableAtts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TableAtts->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->on_CBoxItems_currentIndexChanged(ui->CBoxItems->currentText());
}

AttDialog::~AttDialog()
{
    delete ui;
}

void AttDialog::addAttribute(Attribute *attribute, QString parent)
{
    if(!parent.isEmpty())
        attribute->setChild(true);

    if(currentItem->addAttribute(attribute, parent))
        this->currentChanged();
    else this->on_PButtonAddAtt_clicked();
}

void AttDialog::currentChanged()
{
    this->on_CBoxItems_currentIndexChanged(currentItem->getName());
}

void AttDialog::on_CBoxItems_currentIndexChanged(const QString &itemName)
{
    bool found = false;
    QList<QGraphicsItem*>::iterator it = listItems.begin();
    while(it!=listItems.end() && !found){
        if(((IGraphicItem*)(*it))->getName() == itemName){
            currentItem = (IGraphicItem*)(*it);
            found = true;
        }
        else it++;
    }
    ui->labelItem->setText("Item - "+currentItem->getType());
    QList<Attribute*> tmp = currentItem->getAllAttributes();
    ui->TableAtts->clear();
    QStringList tableHeader;
    tableHeader<<"Name"<<"Optional"<< "Multivalued";
    ui->TableAtts->setHorizontalHeaderLabels(tableHeader);
    ui->TableAtts->setRowCount(0);
    foreach(Attribute* att, tmp)
        att->addToTable(this->ui->TableAtts);
}

void AttDialog::on_PButtonAddAtt_clicked()
{
    error->setError(Error::NoError);
    AttDialogAdd *att = new AttDialogAdd(error, NULL, this);
    att->show();
}

void AttDialog::on_PButtonRemoveAtt_clicked()
{
    QList<QTableWidgetItem *> tmp = ui->TableAtts->selectedItems();
    if(!tmp.isEmpty()){
        currentItem->removeAttribute(tmp.first()->text().remove("      "));
        currentChanged();
    }
}

void AttDialog::on_buttonBox_rejected()
{
    MainWindow *main = ((MainWindow*)this->parent());
    foreach(QGraphicsItem* item, listItems)
        main->deleteItem((IGraphicItem*)item);
    foreach(QGraphicsItem* item, backup)
        main->addItem((IGraphicItem*)item);
    this->close();
}

void AttDialog::on_buttonBox_accepted()
{
    this->close();
}

void AttDialog::on_upPushButton_clicked()
{
    if (currentItem != NULL){       //IGraphicItem that is selected on the ComboBox
        int row = ui->TableAtts->currentRow();
        if (row >= 0){
            int pos = row-1;
            if (pos >= 0){
                currentItem->moveAttribute(ui->TableAtts->item(row,0)->text(),row,pos,ui->TableAtts);
                ((MainWindow*)this->parent())->setSaved(false);
            }
        }
    }
}

void AttDialog::on_downPushButton_clicked()
{
    if (currentItem != NULL){       //IGraphicItem that is selected on the ComboBox
        int row = ui->TableAtts->currentRow();
        if (row >= 0){
            int pos = row+1;
            if (pos < ui->TableAtts->rowCount()){
                currentItem->moveAttribute(ui->TableAtts->item(row,0)->text(),row,pos,ui->TableAtts);
                ((MainWindow*)this->parent())->setSaved(false);
            }
        }
    }
}
