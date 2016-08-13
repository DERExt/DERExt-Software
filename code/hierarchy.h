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

    QString name = "Hierarchy";
    QString typeName;
    Error * error;
    bool exclusive, total;
    QList<Entity*> entities;

public:
    Hierarchy(QString nametype, bool exclusive, bool total, Error * error, QList<Entity*> entities);

    QString getName();
    void setName(QString name);
    bool addAttribute(Attribute *attribute, QString parent, Error *error);
    bool removeAttribute(QString attributesName);
    QList<Attribute *> getAllAttributes();
    QList<Attribute *> getAttributes();
    QList<Attribute *> getAttributesToSave();
    QList<Cardinality *> getCardinalities();
    QDomElement *getXML(QDomDocument *document, QString tagName);
    QList<Entity *> getEntities();
    void setEntities(QList<Entity*> entities);
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget * table);

    //manejo de acciones referenciales y tipos de matching. cada una tiene un listado separado
    QString getOption(Entity * entity, QList<QString> options);
    QList<QString> getOnDelete();
    void setOnDelete(QList<QString> opts);
    QList<QString> getOnUpdate();
    void setOnUpdate(QList<QString> opts);
    QList<QString> getMatch();
    void setMatch(QList<QString> opts);
    void setRolename(QString rn);
    QString getRolename();

    QList<Relationship *> getFKs();
    Hierarchy *getCopy();
    QString getSQL();
    QString getDerivation();
    void addToTableSQL(QTableWidget *table);


private:
    bool alreadyExists(QString name);
    QList<QString> copyList(QList<QString*> list);
    void copyList(QList<QString*> & list, QList<QString>opts);

    // ERItem interface
public:
    QDomElement *getXML(QDomDocument *document);
};

#endif // HIERARCHY_H



