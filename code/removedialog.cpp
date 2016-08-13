#include "removedialog.h"
#include "ui_removedialog.h"

RemoveDialog::RemoveDialog(QList<QGraphicsItem *> items, QWidget *parent) :QDialog(parent),ui(new Ui::RemoveDialog)
{
    this->items = items;
    ui->setupUi(this);
    foreach(QGraphicsItem *item, items)
        ui->itemsCBox->addItem(((IGraphicItem*)item)->getName());
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
            mw->deleteItem(currentItem);
            mw->setSaved(false);
        }
    }
    this->close();
}

void RemoveDialog::on_buttonBox_rejected()
{
    this->close();
}
