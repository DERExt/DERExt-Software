#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "attribute.h"
#include "error.h"
#include "cardinality.h"
#include "eritem.h"
#include "fkcombobox.h"
class Entity;

#include <QtCore>
#include <QtWidgets>

class Hierarchy: public ERItem
{
protected:

    QString name;
    QString typeName;
    Error * error;
    bool exclusive, total;
    QList<Entity*> entities;

public:
    Hierarchy(QString nametype, bool exclusive, bool total,
              Error * error, QList<Entity*> entities);

    QString getName();
    void setName(QString name);
    Entity* getFather();
    QDomElement *getXML(QDomDocument *document, QString tagName);
    QList<Entity *> getEntities();
    void setEntities(QList<Entity*> entities);
    QList<Entity *> getSubtypes();
    Attribute *getPrimaryKey();
    bool isExclusive();
    bool isTotal();
    QString getTypeName();
    QString getDerivation();


    bool addAttribute(Attribute *attribute, QString parent, Error *error);
    bool removeAttribute(QString attributesName);
    QList<Attribute *> getAllAttributes();
    QList<Attribute *> getAttributes();
    QList<Attribute *> getAttributesToSave();
    QList<Cardinality *> getCardinalities();
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget * table);
    QList<Relationship *> getFKs();
    QString getSQL();
    void addToTableSQL(QTableWidget *table);

public:
    QDomElement *getXML(QDomDocument *document);

};

#endif // HIERARCHY_H



