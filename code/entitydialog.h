#ifndef ENTITYDIALOG_H
#define ENTITYDIALOG_H

#include "error.h"
#include "igraphicitem.h"
#include "entityitem.h"
#include "mainwindow.h"

#include <QDialog>
#include <QGraphicsItem>
#include <QList>
#include <QString>
#include <algorithm>

namespace Ui {
class entityDialog;
}

class entityDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {Create, Modify } mode;

    explicit entityDialog(QList<QGraphicsItem *> items, QWidget *parent = 0);
    ~entityDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_checkBox_stateChanged(int state);
    void on_modeComboBox_currentIndexChanged(int index);
    void on_entitiesComboBox_currentIndexChanged(int index);

    void on_check_sub_clicked(bool checked);

private:
    Ui::entityDialog *ui;
    QList<QGraphicsItem*> listItems, backup;
    Error *error;

    bool exists(QString name);
    EntityItem *getCurrentEntity();
    int rshipCount(QString rshipName);
};

#endif // ENTITYDIALOG_H
