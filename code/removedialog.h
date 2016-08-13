#ifndef REMOVEDIALOG_H
#define REMOVEDIALOG_H

#include "igraphicitem.h"
#include "mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QList>

#include <QDebug>

namespace Ui {
class RemoveDialog;
}

class RemoveDialog : public QDialog
{
    Q_OBJECT

    QList<QGraphicsItem *> items;
    
public:
    explicit RemoveDialog(QList<QGraphicsItem *> items, QWidget *parent);
    ~RemoveDialog();
    
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::RemoveDialog *ui;
};

#endif // REMOVEDIALOG_H
