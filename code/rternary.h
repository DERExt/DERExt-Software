
#ifndef RTERNARY_H
#define RTERNARY_H

#include "relationship.h"
#include "entity.h"
#include "cardinality.h"

class RTernary : public Relationship
{
public:
    RTernary(QString nameRship, Error * error, QList<Entity*> ents, QList<Cardinality*> cards);

    Relationship *getCopy();
    QString getSQL();
    QString getDerivation();
    void addToTableSQL(QTableWidget *table);
    QList<Relationship *> getFKs();
    QDomElement * getXML(QDomDocument *document);
private:
    QList<QString> getPKs(bool derivELE);
    QString joinPK(Entity * ent, bool derivELE);
    QString getAssertions();
    void getOthersIndex(int selected, int & o1, int & o2);
    QString getAssertion(int ent);
};

#endif // RTERNARY_H
