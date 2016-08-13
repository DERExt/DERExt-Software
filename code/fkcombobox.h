#ifndef FKCOMBOBOX_H
#define FKCOMBOBOX_H

#include <QtWidgets>
#include "relationship.h"

class FKComboBox: public QComboBox
{
    Q_OBJECT

    QString * opt;
    QWidget *parent;

public:
    FKComboBox(QWidget *parent, QString * opt):QComboBox(parent){
        this->parent=parent;
        this->opt=opt;
        connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(editFK()));
    }

    void addOptions(QList<QString> items){
        QString aux = opt->toLower();
        int index = 0;
        foreach(QString s, items){
            if (s.toLower()==aux){
                index = items.indexOf(s);
            }
            this->addItem(s);
        }
        this->setCurrentIndex(index);
    }

public slots:
    void editFK(){
        opt->clear();
        opt->append(this->currentText().toLower());
    }
};

#endif // FKCOMBOBOX_H
