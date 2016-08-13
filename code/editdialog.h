#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include "attribute.h"
#include "attsimple.h"
#include "attcomp.h"

#include <QDialog>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

    Attribute *att;

public:
    explicit EditDialog(Attribute *att, QWidget *parent = 0);
    ~EditDialog();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    void initialize();

private:
    Ui::EditDialog *ui;
};

#endif // EDITDIALOG_H
