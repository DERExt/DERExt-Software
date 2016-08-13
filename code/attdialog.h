#ifndef ATTDIALOG_H
#define ATTDIALOG_H

#include "igraphicitem.h"
#include "attribute.h"
#include "error.h"

#include <QDialog>
#include <QTableWidget>
#include <QGraphicsItem>

namespace Ui {
class AttDialog;
}

class AttDialog : public QDialog
{
    Q_OBJECT

    QList<QGraphicsItem*> listItems, backup;
    IGraphicItem *currentItem;
    Error *error;

public:
    explicit AttDialog(QList<QGraphicsItem *> listItems, QWidget *parent = 0);
    ~AttDialog();
    void addAttribute(Attribute *attribute, QString parent);
    void currentChanged();

private slots:
    void on_CBoxItems_currentIndexChanged(const QString &itemName);
    void on_PButtonRemoveAtt_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

    void on_upPushButton_clicked();

    void on_downPushButton_clicked();

public slots:
    void on_PButtonAddAtt_clicked();

private:
    Ui::AttDialog *ui;
};

#endif // ATTDIALOG_H
