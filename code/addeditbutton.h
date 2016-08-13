#ifndef ADDEDITBUTTON_H
#define ADDEDITBUTTON_H

#include "attribute.h"
#include "editdialog.h"

#include <QPushButton>

class AddEditButton : public QPushButton
{
    Q_OBJECT

    Attribute *att;
    QWidget *parent;

public:
    explicit AddEditButton(QIcon icon, QString text, Attribute *att, QWidget *parent);
    
public slots:
    void editAttribute();
};

#endif // ADDEDITBUTTON_H
