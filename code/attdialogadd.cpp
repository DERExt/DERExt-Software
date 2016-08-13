#include "attdialogadd.h"
#include "ui_attdialogadd.h"
#include "attdialog.h"

AttDialogAdd::AttDialogAdd(Error *error, Attribute *complexAttribute, QWidget *parent) :QDialog(parent), ui(new Ui::AttDialogAdd)
{
    ui->setupUi(this);
    ui->amountLabel->setHidden(true);
    ui->amoutSpinBox->setHidden(true);
    this->error = error;
    this->complex = complexAttribute;
    this->setWindowTitle("Add new attribute");
    this->ui->nameLEdit->setFocus();
}

AttDialogAdd::~AttDialogAdd()
{
    delete ui;
}

void AttDialogAdd::on_buttonBox_accepted()
{
    error->setError(Error::NoError);
    Attribute *att;

    if(!ui->nameLEdit->text().isEmpty()){
        if(ui->typeButtonGroup->checkedButton()->text() == "Simple"){
            att = new AttSimple(ui->nameLEdit->text(), ui->nullChBox->isChecked(), ui->multivaluedCBox->isChecked(), false, false, false);
        }
        if(ui->typeButtonGroup->checkedButton()->text() == "Compound"){
            att = new AttComp(ui->nameLEdit->text(), ui->nullChBox->isChecked(), ui->amoutSpinBox->value(), ui->multivaluedCBox->isChecked(), false, false, false);
        }
    }
    else error->setError(Error::EmptyNameAttribute);
    if(error->getError() == Error::NoError){
        QString tmp("");
        if(this->complex!=NULL)
            tmp = this->complex->getName();
        ((AttDialog*)this->parent())->addAttribute(att, tmp);
    }
    else ((AttDialog*)this->parent())->on_PButtonAddAtt_clicked();
}

bool AttDialogAdd::canBeAdded(QList<Attribute *> attributesList, QString name) const
{
    foreach(Attribute *att, attributesList){
        if(att->getName() == name)
            return false;
    }
    return true;
}

void AttDialogAdd::on_simpleButton_clicked()
{
    ui->amountLabel->setHidden(true);
    ui->amoutSpinBox->setHidden(true);
    ui->nameLabel->setHidden(false);
    ui->nameLEdit->setHidden(false);
}

void AttDialogAdd::on_compoundButton_clicked()
{
    ui->amountLabel->setHidden(false);
    ui->amoutSpinBox->setHidden(false);
    ui->nameLabel->setHidden(false);
    ui->nameLEdit->setHidden(false);
}
