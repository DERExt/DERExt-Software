#ifndef ADDPUSHBUTTON
#define ADDPUSHBUTTON

#include "attdialogadd.h"

#include <QPushButton>
#include <QWidget>


class AddPushButton : public QPushButton
{
    Q_OBJECT

    QWidget *parent;
    Attribute *complex;

public:
    AddPushButton(QIcon icon, QString text, Attribute *complexAttribute, QWidget *parent):QPushButton(icon, text, parent)
    {
        this->parent = parent;
        this->complex = complexAttribute;
        connect(this, SIGNAL(clicked()), this, SLOT(addAttribute()));
    }

private slots:
    void addAttribute()
    {
        AttDialogAdd *att = new AttDialogAdd(new Error(parent), this->complex, this->parent);
        att->show();
    }

};

#endif // ADDPUSHBUTTON

