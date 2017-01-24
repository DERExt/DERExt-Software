#ifndef HIERARCHYDIALOG_H
#define HIERARCHYDIALOG_H

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
#include "hierarchyitem.h"
#include <algorithm>

namespace Ui {
class HierarchyDialog;
}

class HierarchyDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {Create=0, Modify=1} mode;

    explicit HierarchyDialog(QList<QGraphicsItem *> items, QWidget *parent = 0);
    ~HierarchyDialog();


    bool isFatherValid(EntityItem *item);
    bool isSonValid(EntityItem *item);
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_exclusiveBool_clicked(bool checked);
    void on_pushAdd_clicked();
    void on_pushRemove_clicked();
    void on_modeComboBox_currentIndexChanged(int index);
    void on_sup_currentIndexChanged(int index);
    void on_lineEdit_editingFinished();

private:
    Ui::HierarchyDialog *ui;
    QList<QGraphicsItem*> sceneItems;
    QList<QGraphicsItem*> backup;
    Error *error;
    EntityItem *auxModify;

    HierarchyItem *createHierarchy();
    HierarchyItem *getHierarchy();

    void removeHierarchy(QList<EntityItem*> list);

};

#endif // HIERARCHYDIALOG_H
