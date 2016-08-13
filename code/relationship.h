#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include "attribute.h"
#include "error.h"
#include "cardinality.h"
#include "eritem.h"
#include "fkcombobox.h"
class Entity;

#include <QtCore>
#include <QtWidgets>

class Relationship: public ERItem
{
protected:
    QString name, roleName;
    Error * error;
    QList<Attribute*> atts;
    QList<Cardinality*> cards;
    QList<Entity*> entities;
    QList<QString*> onDelete, onUpdate, matching;

public:
    Relationship(QString nameRship, Error * error, QList<Entity*> entities, QList<Cardinality*> cards);

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
    QString getRolename(){ return roleName; }

    virtual QList<Relationship*> getFKs()=0;
    virtual Relationship *getCopy()=0;
    virtual QString getSQL()=0;
    virtual QString getDerivation()=0;
    virtual void addToTableSQL(QTableWidget *table)=0;

private:
    bool alreadyExists(QString name);
    QList<QString> copyList(QList<QString*> list);
    void copyList(QList<QString*> & list, QList<QString>opts);
};

#endif // RELATIONSHIP_H
