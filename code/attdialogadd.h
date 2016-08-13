#ifndef ATTDIALOGADD_H
#define ATTDIALOGADD_H

#include "error.h"
#include "attribute.h"
#include "attsimple.h"
#include "attcomp.h"

#include <QDialog>


namespace Ui {
class AttDialogAdd;
}

class AttDialogAdd : public QDialog
{
    Q_OBJECT

    Error *error;
    Attribute *complex;

public:
    explicit AttDialogAdd(Error *error, Attribute *complexAttribute, QWidget *parent = 0);
    ~AttDialogAdd();

private slots:
    void on_buttonBox_accepted();
    void on_simpleButton_clicked();
    void on_compoundButton_clicked();

private:
    bool canBeAdded(QList<Attribute*> attributesList, QString name) const;

private:
    Ui::AttDialogAdd *ui;
};

#endif // ATTDIALOGADD_H
