#ifndef ATTDIALOGEDIT_H
#define ATTDIALOGEDIT_H

#include "igraphicitem.h"
#include "error.h"
#include "attribute.h"

#include <QDialog>

namespace Ui {
class AttDialogEdit;
}

class AttDialogEdit : public QDialog
{
    Q_OBJECT

    Error *error;
    QList<QGraphicsItem*> listItems, backup;
    IGraphicItem *item;
    QList<QString> names;

public:
    explicit AttDialogEdit(QList<QGraphicsItem *> listItems, QWidget *parent = 0);
    ~AttDialogEdit();

private slots:
    void on_primaryKeyPButton_clicked();
    void on_secondaryKeyPButton_clicked();
    void on_itemComboBox_currentIndexChanged(const QString &itemName);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_eraseKeyPButton_clicked();

    void on_itemComboBox_currentIndexChanged(int index);

private:
    void setAttributes(QList<Attribute *> atts);
    Attribute *getAttribute(int row);

private:
    Ui::AttDialogEdit *ui;
};

#endif // ATTDIALOGEDIT_H
