#include "hierarchy.h"
#include "entity.h"

Hierarchy::Hierarchy(QString typeName, bool exclusive, bool total, Error *error, QList<Entity *> entities)
{
    this->name = "Hierarchy " + entities.at(0)->getName();
    this->error = error;
    this->typeName = typeName;
    this->entities.append(entities);
    this->exclusive = exclusive;
    this->total = total;
    int longEnt = entities.count();
    entities.at(0)->setHierarchy(this);
    for(int i = 1; i <entities.count(); i++)
        entities.at(i)->setHierarchySon(this);
}

QString Hierarchy::getName()
{
    return name;
}

QString Hierarchy::getTypeName()
{
    return typeName;
}

void Hierarchy::setName(QString name)
{
    this->name = name;
}
Attribute *Hierarchy::getPrimaryKey(){
    if(!entities.isEmpty())
        return entities.at(0)->getPrimaryKey();
    return NULL;
}

bool Hierarchy::isExclusive(){
    return exclusive;
}

bool Hierarchy::isTotal(){
    return total;
}

Entity *Hierarchy::getFather()
{
    if(!entities.isEmpty())
        return entities.at(0);
}

bool Hierarchy::addAttribute(Attribute *, QString , Error *)
{
    /*No se implementa*/
}
bool Hierarchy::removeAttribute(QString attributesName)
{
    /*No se implementa*/
}

QList<Attribute *> Hierarchy::getAllAttributes()
{
    /*No se implementa*/
}

QList<Attribute *> Hierarchy::getAttributes()
{
    /*No se implementa*/
}
QList<Attribute *> Hierarchy::getAttributesToSave()
{
    return getAttributes();
}

QList<Cardinality *> Hierarchy::getCardinalities()
{
    /*No se implementa*/
}

QList<Entity *> Hierarchy::getEntities()
{
    return this->entities;
}

void Hierarchy::setEntities(QList<Entity *> entities)
{
    this->entities.clear();
    this->entities.append(entities);
}

bool Hierarchy::moveAttribute(QString attribute, int row, int newPos, QTableWidget *table)
{
    /*No se implementa*/
}

QDomElement *Hierarchy::getXML(QDomDocument *document, QString tagName)
{
    QDomElement * tmp = new QDomElement(document->createElement(tagName));
    tmp->setAttribute("name", this->getName());
    if(exclusive){
        tmp->setAttribute("exclusive", QString("true"));
        tmp->setAttribute("typeName", typeName);
    }
    else{
        tmp->setAttribute("exclusive", QString("false"));
        tmp->setAttribute("typeName", " ");
    }

    if(total)
        tmp->setAttribute("total", QString("true"));
    else
        tmp->setAttribute("total", QString("false"));

    tmp->setAttribute("cont", QString::number(entities.size()));
    for(int i = 0; i< entities.size(); i++)
        tmp->setAttribute("name"+QString::number(i),((ERItem*)entities.at(i))->getName());
    return tmp;
}

QList<Entity *> Hierarchy::getSubtypes(){
    QList<Entity*> aux;
    for(int i = 1; i<entities.count(); i++)
        aux << entities.at(i);
    return aux;
}

QList<Relationship *> Hierarchy::getFKs()
{
    /*No se implementa*/
}

QString Hierarchy::getSQL()
{
    /*No se implementa*/
}

QString Hierarchy::getDerivation()
{
    QString aux("");
    Entity* ent = this->getFather();
    Entity*son;
    for(int i = 1; i < entities.count(); i++){
        son = entities.at(i);
        QString sonName = son->getName();
        aux.append("ESUN_"+sonName+"["+sonName+"."+son->getPrimaryKey()->derive());
        aux.append(";;;;;");
        aux.append(ent->getName()+"."+ent->getPrimaryKey()->derive());
        aux.append("]");
    }
    return aux;
}

void Hierarchy::addToTableSQL(QTableWidget *table)
{
    /*No se implementa*/
}

QDomElement *Hierarchy::getXML(QDomDocument *document){
    /*Ya esta implementado, este metodo es
     * necesario colocarlo porque pertenece
     * a la interfaz pero no es utilizado
     * aqui*/
}
