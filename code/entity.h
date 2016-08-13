#ifndef ENTITY_H
#define ENTITY_H

#include "attribute.h"
#include "attcomp.h"
#include "error.h"
#include "eritem.h"
#include "rbinary.h"

#include <QList>

class Entity: public ERItem
{
protected:
    Attribute* primaryKey;
    QList<Attribute*> atts, secondaryKeys, compoundSecondary, partialSK;
    QList<Relationship*> N1rships;  //for SQL
    QString name;
    Error *error;
    int altKey;
    Entity * father;

public:
    enum {secondaryKey = 0, simpleAtts = 1, compAtts = 2, multAtts = 3};

public:
    Entity(QString entityName, Error *error);
    ~Entity();
    QString getName();
    void setName(QString name);
    Attribute *getPrimaryKey() const;
    QString getPKder(bool isOpt=0);
    bool setPrimaryKey(Attribute *attribute);
    bool setSecondaryKey(Attribute *secondaryKey);
    bool removeSecondaryKey(QString secondaryKey);
    virtual bool addAttribute(Attribute *attribute, QString parent, Error *error);
    bool removeAttribute(QString attributesName);
    QList<Attribute *> getAllAttributes();
    QList<Attribute *> getAttributes();
    QList<Attribute *> getSecondaryKeys();
    bool setSecondaryKeyComp(QList<Attribute *> atts);
    QList<Attribute *> getAttributesToSave();
    QList<QString> getSKToSave();
    void addCompSK(QString name);
    void generateCompSK();

    void setFather(Entity*item);
    Entity* getFather();
    void deleteFather();

    bool moveAttribute(QString attribute, int row, int newPos, QTableWidget *table);
    void addN1Rship(Relationship * rship);

    //methods that might be overriden by subclasses of Entity
    virtual QString getSQL();
    virtual QString getDerivation();
    virtual Entity *getCopy();
    virtual QList<Relationship *> getFKs();
    virtual QDomElement * getXML(QDomDocument *document);


private:
    bool alreadyExists(QString name);
    bool isChild(Attribute *attribute);
    Relationship * getRship(Entity * ent);
};

#endif // ENTITY_H
