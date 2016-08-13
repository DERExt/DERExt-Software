#ifndef ERITEM_H
#define ERITEM_H

#include "error.h"
#include <QtCore>
#include <QDomElement>
class Relationship;
class Hierarchy;

class ERItem
{
public:
    virtual QString getName()=0;
    virtual void setName(QString name)=0;
    virtual bool addAttribute(Attribute *attribute, QString parent, Error *error)=0;
    virtual bool removeAttribute(QString attributesName)=0;
    virtual QList<Attribute *> getAllAttributes()=0;
    virtual QList<Attribute *> getAttributes()=0;
    virtual QString getSQL()=0;
    virtual QList<Relationship*> getFKs()=0;
    virtual QDomElement * getXML(QDomDocument *document)=0;
    virtual QString getDerivation() =0;
};

#endif // ERITEM_H
