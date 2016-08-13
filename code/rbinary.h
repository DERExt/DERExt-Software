#ifndef RBINARY_H
#define RBINARY_H
#include "relationship.h"
#include "entity.h"

class RBinary: public Relationship
{
public:
    RBinary(QString nameRship, Error * error, QList<Entity *> ents, QList<Cardinality *> cards);
    Relationship *getCopy();
    QString getSQL();
    void addToSQL(QString &tmp, int ent, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints);
    QString getDerivation();
    void addToTableSQL(QTableWidget *table);
    QList<Relationship *> getFKs();
    QDomElement *getXML(QDomDocument *document);

private:
    QString getAssertion(int ent);
    void addRolename(QString &tmp, int ent);
    bool isOptional(QString entityName);
};

#endif // RBINARY_H
