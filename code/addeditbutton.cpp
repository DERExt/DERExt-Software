#include "addeditbutton.h"

AddEditButton::AddEditButton(QIcon icon, QString text, Attribute *att, QWidget *parent):QPushButton(icon, text, parent)
{
    this->parent = parent;
    this->att = att;
    connect(this, SIGNAL(clicked()), this, SLOT(editAttribute()));
}

void AddEditButton::editAttribute()
{
    EditDialog *aux = new EditDialog(att, parent);
    aux->show();
}
