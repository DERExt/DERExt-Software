#ifndef ERROR_H
#define ERROR_H

#include <QWidget>

class Error
{
    QWidget *parent;

public:
    enum ErrorType{NoTab, NoItems, InvalidPK, PrimarySecondary, SecondaryChild, DuplicateEntity,
                   UndefinedAttributes, InvalidFile, NoError, NoPrimaryKey, EmptyNameAttribute,
                   EmptyNameEntity, NameTaken, EmptyNameRship, Cardinality, SameRships, WeakEntity,
                  TypeHrchy, NameSub, NoEntity, EmptyTypeName } error;

    Error(QWidget *parent);
    void setError(ErrorType errorType);
    void setParent(QWidget *parent);
    ErrorType getError() const;

private:
    void AnounceError();
};

#endif // ERROR_H
