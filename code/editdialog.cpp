#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(Attribute *att, QWidget *parent):QDialog(parent), ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit Attribute");
    this->att = att;
    ui->OptionOne->setVisible(false);
    ui->OptionTwo->setVisible(false);
    ui->OptionThree->setVisible(false);
    ui->spinBox->setVisible(false);
    ui->spinBox_2->setVisible(false);
    ui->spinBox_3->setVisible(false);
    this->initialize();
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::initialize()
{
    if(att->getChildren().size()>0){
        ui->label->setVisible(false);
        ui->comboBox->setVisible(false);
        ui->OptionOne->setText("Add to main table");
        ui->OptionOne->setVisible(true);
        ui->OptionTwo->setText("Create a new type");
        ui->OptionTwo->setVisible(true);
    }
    else{
        if(att->isMultivalued()){
            ui->OptionOne->setText("Add to main table");
            ui->OptionOne->setVisible(true);
            ui->spinBox->setVisible(true);
            ui->OptionTwo->setText("Create an array");
            ui->OptionTwo->setVisible(true);
            ui->OptionThree->setText("Create new table");
            ui->OptionThree->setVisible(true);
        }
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(((AttSimple*)att)->getType()));
        ui->spinBox_2->setValue(((AttSimple*)att)->cantA());
        ui->spinBox_3->setValue(((AttSimple*)att)->cantB());
        ui->spinBox->setValue(((AttSimple*)att)->cantC());
    }
    int a = att->getOptionSQL();
    if(a == 1)
        ui->OptionOne->setChecked(true);
    if(a == 2)
        ui->OptionTwo->setChecked(true);
    if(a == 3)
        ui->OptionThree->setChecked(true);

}

void EditDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(ui->comboBox->currentText()=="varchar" || ui->comboBox->currentText()=="char"){
        ui->spinBox_3->setVisible(false);
        ui->spinBox_2->setVisible(true);
    }
    else if(ui->comboBox->currentText()=="numeric"){
        ui->spinBox_2->setVisible(true);
        ui->spinBox_3->setVisible(true);
    }
    else{
        ui->spinBox_2->setVisible(false);
        ui->spinBox_3->setVisible(false);
    }
}

void EditDialog::on_buttonBox_accepted()
{
    if(att->getChildren().isEmpty())
        ((AttSimple*)att)->setType(ui->comboBox->currentText(), ui->spinBox_2->value(), ui->spinBox_3->value(), ui->spinBox->value());

    att->setOptionSQL(1);
    if(ui->OptionTwo->isChecked())
        att->setOptionSQL(2);
    if(ui->OptionThree->isChecked())
        att->setOptionSQL(3);
}

void EditDialog::on_buttonBox_rejected()
{

}
