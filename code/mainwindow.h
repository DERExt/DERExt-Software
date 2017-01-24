#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "error.h"
#include "entityitem.h"
#include "attsimple.h"
#include "attdialog.h"
#include "attcomp.h"
#include "entity.h"
#include "weakentity.h"
#include "graphicview.h"
#include "removedialog.h"
#include "sqldialog.h"
#include "removedialog.h"
#include "relationshipdialog.h"
#include "relationshipitem.h"
#include "entitydialog.h"
#include "rternaryitem.h"
#include "hierarchydialog.h"
#include "associationdialog.h"

#include <QGraphicsView>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QtXml/QDomDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QShortcut>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Error *error;

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool deleteItem(IGraphicItem *item);
    Entity *parseEntityELE(QString input, QList<Entity*> loadedItems, QList<QString> & weaks);
    Relationship *parseRelationshipELE(QString input, QList<Entity *> loadedItems);
    void parseERItem(QString input, QList<ERItem *> &loadedItems, QList<Entity *> &loadedEnts, QList<QString> &weaks);
    void parseAttributes(ERItem * item, QString input);
    bool addItem(IGraphicItem *item);
    void setSaved(bool saved);

private slots:
    void on_addNewTabCommButton_clicked();
    void on_addNewEntityCommButton_clicked();
    void on_addNewAttributeCommButton_clicked();
    void on_getDerivationCommButton_clicked();
    void on_savePButton_clicked();
    void closeCurrentTab();
    void saveTabAs();
    void saveAllTabs();
    void on_openPButton_clicked();
    void on_exitPButton_clicked();
    bool close();
    void on_cleanPButton_clicked();
    void about();
    void on_editKeysCommButton_clicked();
    void on_removeEntityCommButton_clicked();
    void on_getSQLDerivationCommButton_clicked();
    void on_RelationshipCommButton_clicked();

    void on_RelationshipCommButton_2_clicked();
    void on_associationButton_clicked();


protected:
    void closeEvent(QCloseEvent *event);

private:
    void makeConnections();
    bool closeTab(QMdiSubWindow *sub);
    Entity * getEnt(QList<Entity *> items, QString name, bool pk = 0);
    QGraphicsItem * getItem(QList<QGraphicsItem*> items, QString name);
    void addN1Rship(Relationship * rship);
    void loadRelationships(QList<QGraphicsItem*> & items, QList<QDomElement> weakEntities);
    void loadEntities(QList<QGraphicsItem*> & items);

    QList<QDomElement> aggregations;
    QList<QDomElement> nonReadyRelationships;
private:
    Ui::MainWindow *ui;
    void loadItem(QDomElement element, QList<QGraphicsItem *> &items, QList<QDomElement> &weakEntities, bool loadELE=0);
};

#endif // MAINWINDOW_H
