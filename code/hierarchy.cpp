#include "hierarchy.h"
#include "entity.h"

Hierarchy::Hierarchy(QString typeName, bool exclusive, bool total, Error *error, QList<Entity *> entities)
{
    this->name = "Hierarchy";
    this->error = error;
    this->typeName = typeName;
    this->entities.append(entities);
    this->exclusive = exclusive;
    this->total = total;
}

QString Hierarchy::getName()
{
    return name;
}

void Hierarchy::setName(QString name)
{
    this->name = name;
}

bool Hierarchy::addAttribute(Attribute *, QString , Error *)
{
    /*No se implementa*/
}

/**
 * Igual que el agregado, la suposicion de los atributos disponibles.
 */
bool Hierarchy::removeAttribute(QString attributesName)
{

}

QList<Attribute *> Hierarchy::getAllAttributes()
{

}

QList<Attribute *> Hierarchy::getAttributes()
{

}

/**
 * Lo mismo, si se aceptan otros tipos de atributos este metodo debe cambiar.
 */
QList<Attribute *> Hierarchy::getAttributesToSave()
{
    return getAttributes();
}

QList<Cardinality *> Hierarchy::getCardinalities()
{

}

QList<Entity *> Hierarchy::getEntities()
{
    return this->entities;
}

void Hierarchy::setEntities(QList<Entity *> entities)
{
    this->entities = entities;
}

bool Hierarchy::moveAttribute(QString attribute, int row, int newPos, QTableWidget *table)
{

}

//given the entity name, return the option corresponding to it in such list
QString Hierarchy::getOption(Entity *entity, QList<QString> options)
{

}

QList<QString> Hierarchy::getOnDelete(){ }

void Hierarchy::setOnDelete(QList<QString> opts){ }

QList<QString> Hierarchy::getOnUpdate(){ }

void Hierarchy::setOnUpdate(QList<QString> opts){ }

QList<QString> Hierarchy::getMatch(){ }

void Hierarchy::setMatch(QList<QString> opts){ }

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

    //for (int j = 0; j < cards.size(); j++){
        //tmp->setAttribute("min"+QString::number(j),cards.at(j)->getMin());
        //tmp->setAttribute("max"+QString::number(j),cards.at(j)->getMax());
    //}
    //if (entities.size() < 3)    tmp->setAttribute("rolename",roleName);

    //foreach(Attribute * att, getAttributesToSave())
        //att->addToXML(document,tmp,QString(""));
    return tmp;
}

void Hierarchy::setRolename(QString rn)
{

}

QString Hierarchy::getRolename(){
    return " ";
}

QList<Relationship *> Hierarchy::getFKs()
{

}

Hierarchy *Hierarchy::getCopy()
{

}

QString Hierarchy::getSQL()
{

}

QString Hierarchy::getDerivation()
{

}

void Hierarchy::addToTableSQL(QTableWidget *table)
{

}

bool Hierarchy::alreadyExists(QString name)
{
    if(name.isEmpty())
        return false;
    else
        foreach (Entity *item, entities) {
            if(item->getName() == name)
                return true;
        }
    return false;
}

QList<QString> Hierarchy::copyList(QList<QString *> list)
{
    QList<QString> result;
    foreach(QString*s,list)
        result << *s;
    return result;
}

void Hierarchy::copyList(QList<QString *> &list, QList<QString> opts)
{
    if (list.size()==opts.size())
        for (int i = 0; i < list.size(); i++){
            list.at(i)->clear();
            list.at(i)->append(opts.at(i));
        }
}

QDomElement *Hierarchy::getXML(QDomDocument *document)
{
    /*implemento el otro metodo*/
}
