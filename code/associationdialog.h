#ifndef ASSOCIATIONDIALOG_H
#define ASSOCIATIONDIALOG_H

#include "error.h"
#include "igraphicitem.h"
#include "mainwindow.h"
#include "runaryitem.h"
#include "rbinaryitem.h"
#include "rternaryitem.h"
#include "cardinality.h"

#include <QDialog>
#include <QGraphicsItem>
#include <QVBoxLayout>
#include <QWidget>
#include "entityitem.h"

#include <QDialog>
#include <relationshipitem.h>
#include <QDebug>

namespace Ui {
class associationdialog;
}

class associationdialog : public QDialog
{
    Q_OBJECT

public:
    explicit associationdialog(QList<QGraphicsItem *> items, QWidget *parent = 0);
    ~associationdialog();

    enum Mode {Create=0, Modify=1} mode;

private slots:

    void on_pushAdd_clicked();
    void on_modeComboBox_currentIndexChanged(int index);
    void on_buttonBox_accepted();

private:
    EntityItem * createAssociation();

    Ui::associationdialog *ui;
    QList<QGraphicsItem*> sceneItems;
    QList<QGraphicsItem*> backup;
    Error *error;
    EntityItem *auxModify;
};

#endif // ASSOCIATIONDIALOG_H
