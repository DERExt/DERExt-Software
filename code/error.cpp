#include "error.h"
#include <QMessageBox>
#include <QCoreApplication>

Error::Error(QWidget *parent)
{
    this->parent = parent;
    this->error = NoError;
}


void Error::AnounceError()
{
    QString message;
    switch(error)
    {
    case NameTaken:  message = "That name is already in use!";
        break;
    case NoTab: message = "There is no active tab!";
        break;
    case NoItems: message = "There are no items!";
        break;
    case EmptyNameAttribute: message = "Attribute's name cannot be empty!";
        break;
    case EmptyNameEntity: message = "Entity's name cannot be empty!";
        break;
    case NoPrimaryKey: message = "Some entities do not have a primary key!";
        break;
    case InvalidPK: message = "That attribute cannot be primary key!";
        break;
    case PrimarySecondary: message = "The attribute is already primary key!";
        break;
    case SecondaryChild: message = "The attribute cannot be secondary key!";
        break;
    case UndefinedAttributes: message = "Some attributes are not completely defined!";
        break;
    case InvalidFile: message = "File is not valid!";
        break;
    case DuplicateEntity: message = "The entity already exists!";
        break;
    case EmptyNameRship: message = "Relationship's name cannot be empty.";
        break;
    case Cardinality: message = "Cardinalities are not right.";
        break;
    case SameRships: message = "Entities in relationship cannot be the same.";
        break;
    case WeakEntity: message = "Entity cannot be weak.";
        break;
    case TypeHrchy: message = "Type name cannot be empty.";/**NEW**/
        break;
    case NameSub: message = "Subtype name already used.";/**NEW**/
        break;
    case NoEntity: message = "Select at least two entities.";/**NEW**/
        break;
    case EmptyTypeName: message = "Hierarchy type name cannot be empty.";/**NEW**/
        break;
    case PartHierarchy: message = "The entity is part of a hierarchy.";/**NEW**/
    }

    QMessageBox::critical(parent, "Error", message, QMessageBox::Ok, QMessageBox::Ok);
}

Error::ErrorType Error::getError() const
{
    return this->error;
}

void Error::setError(ErrorType type)
{
    this->error = type;
    if(error != NoError)
        this->AnounceError();
}

void Error::setParent(QWidget *parent)
{
    this->parent = parent;
}

