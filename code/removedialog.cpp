#include "removedialog.h"
#include "ui_removedialog.h"
#include "hierarchyitem.h"
RemoveDialog::RemoveDialog(QList<QGraphicsItem *> items, QWidget *parent) :QDialog(parent),ui(new Ui::RemoveDialog)
{
    this->items = items;
    ui->setupUi(this);
    foreach(QGraphicsItem *item, items){
        if(item->type())
            ui->itemsCBox->addItem(((IGraphicItem*)item)->getName());
    }
}

RemoveDialog::~RemoveDialog()
{
    delete ui;
}

void RemoveDialog::on_buttonBox_accepted()
{
    QString name = ui->itemsCBox->currentText();
    IGraphicItem *currentItem = NULL;
    foreach(QGraphicsItem *item, items)
        if(((IGraphicItem*)item)->getName() == name)
            currentItem = (IGraphicItem*)item;
    if(currentItem!=NULL){
        QMessageBox msgBox(this);
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText("<div align=""center""><b>Everything related to '"+ name +"' will be erased.</b></div>");
        msgBox.setInformativeText(tr("<div align=""center"">Do you want to continue?</div>"));
        msgBox.setIcon(QMessageBox::Question);
        QPushButton *buttonOk = msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
        /*QPushButton *buttonCancel = */msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
        msgBox.exec();
        if((QPushButton*)msgBox.clickedButton()== buttonOk){
            MainWindow * mw = (MainWindow*)this->parent();
            if(((EntityItem*)currentItem)->type() == EntityItem::Type){
                if(((EntityItem*)currentItem)->getHierarchy() != NULL )
                    removeHierarchy(((EntityItem*)currentItem)->getHierarchy());
                else
                    if(((EntityItem*)currentItem)->getHierarchySon() != NULL )
                        removeSpecialHierarchy(((EntityItem*)currentItem)->getHierarchySon());
            }
            mw->deleteItem(currentItem);
            mw->setSaved(false);
        }
    }
    this->close();
}
void RemoveDialog::removeSpecialHierarchy(HierarchyItem *hie){
    MainWindow * mw = (MainWindow*)this->parent();
    QList<EntityItem*> listEnt = hie->getEntities();
    if(listEnt.count() == 2){
        QString name = hie->getName();
        mw->deleteItem(hie);
        foreach(QGraphicsItem *item, items){
            if(((IGraphicItem*)item)->getName() == name)
                mw->deleteItem((IGraphicItem*)item);
        }
    }
}

void RemoveDialog::removeHierarchy(HierarchyItem *hie){
    MainWindow * mw = (MainWindow*)this->parent();
    QList<EntityItem*> listEnt = hie->getEntities();
    for(int i = 1; i < listEnt.count(); i++){
        if(listEnt.at(i)->getHierarchy() != NULL )
            removeHierarchy(listEnt.at(i)->getHierarchy());
        mw->deleteItem(listEnt.at(i));
    }
    QString name = hie->getName();
    mw->deleteItem(hie);
    foreach(QGraphicsItem *item, items){
        if(((IGraphicItem*)item)->getName() == name)
            mw->deleteItem((IGraphicItem*)item);
        }
}

void RemoveDialog::on_buttonBox_rejected()
{
    this->close();
}
