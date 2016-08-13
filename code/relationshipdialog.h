#ifndef RELATIONSHIPDIALOG_H
#define RELATIONSHIPDIALOG_H

#include "error.h"
#include "igraphicitem.h"
#include "mainwindow.h"
#include "runaryitem.h"
#include "rbinaryitem.h"
#include "rternaryitem.h"
#include "relationshipitem.h"
#include "cardinality.h"

#include <QDialog>
#include <QGraphicsItem>
#include <QVBoxLayout>

namespace Ui {
class RelationshipDialog;
}

class RelationshipDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {Create=0, Modify=1} mode;

    explicit RelationshipDialog(QList<QGraphicsItem *> items, QWidget *parent = 0);
    ~RelationshipDialog();

    void setRelationship(QString rshipName);

public slots:
    void on_rtypeComboBox_currentIndexChanged(int index);

private slots:
    void on_buttonBox_accepted();
    void on_min1LineEdit_editingFinished();
    void on_max1LineEdit_editingFinished();
    void on_max2LineEdit_editingFinished();
    void on_min2LineEdit_editingFinished();
    void on_min3LineEdit_editingFinished();
    void on_max3LineEdit_editingFinished();
    void on_modeComboBox_currentIndexChanged(int index);
    void on_buttonBox_rejected();
    void on_nameLineEdit_editingFinished();
    void on_weakRadioButton_clicked();
    void on_weakRadioButton2_clicked();

    void on_rtypeComboBox_activated(const QString &arg1);

    void on_e1ComboBox_activated(const QString &arg1);

private:
    Ui::RelationshipDialog *ui;
    QList<QGraphicsItem*> sceneItems;
    QList<QGraphicsItem*> backup;
    QList<QVBoxLayout*> entitiesUi;
    Error *error;

    //get from UI
    RelationshipItem * createRelationship();
    Cardinality *rightCardinalities(int entity);
    //get from existing items
    EntityItem *getEntity(int entityNumber);
    RelationshipItem *getRelationship();

    void updateIdDependency(bool idDep);
    void updateRshipCardinality(int index, Cardinality *newCard);
    bool differentEntities(QList<EntityItem *> list);
    //changes in UI according to modes and types of rship
    void enableEntity(int index, bool enable);    
    void updateWeakEntUI();
    void updateEntities(int limit);
};

#endif // RELATIONSHIPDIALOG_H
