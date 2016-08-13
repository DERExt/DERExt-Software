#ifndef WEAKENTITY_H
#define WEAKENTITY_H
#include "entity.h"

class WeakEntity : public Entity
{
private:
    Entity * strongEntity;
public:
    WeakEntity(QString entityName, Error *error, Entity * strongEntity);
    bool addAttribute(Attribute *attribute, QString parent, Error *error);
    QString getDerivation();
    Entity *getCopy();
    Entity *getStrongEntity();
    QString getSQL();
    QDomElement *getXML(QDomDocument *document);
};

#endif // WEAKENTITY_H
