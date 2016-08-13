#ifndef SQLDIALOG_H
#define SQLDIALOG_H

#include "attribute.h"
#include "entity.h"
#include "entityitem.h"

#include <QDialog>
#include <QDebug>

namespace Ui {
class SQLDialog;
}

class SQLDialog : public QDialog
{
    Q_OBJECT

    QList<ERItem*> items;
    Error *error;

public:
    explicit SQLDialog(QList<ERItem*> list, Error *error, QWidget *parent = 0);
    ~SQLDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_savePButton_clicked();
    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    void changeItem(QString name);
    void initializeTables();
    void avoidRepetition(QList<Relationship*>rships, Entity * ent);

private:
    Ui::SQLDialog *ui;
};

#endif // SQLDIALOG_H
