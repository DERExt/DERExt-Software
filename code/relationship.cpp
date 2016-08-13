#include "relationship.h"

Relationship::Relationship(QString nameRship, Error *error, QList<Entity *> entities, QList<Cardinality *> cards)
{
    this->name = nameRship;
    this->error = error;
    this->entities.append(entities);
    this->cards.append(cards);
    for(int i = 0; i< entities.size(); i++){    //initialize options
        onDelete << new QString("restrict");
        onUpdate << new QString("restrict");
        matching << new QString("simple");
    }
}

QString Relationship::getName()
{
    return name;
}

void Relationship::setName(QString name)
{
    this->name = name;
}

bool Relationship::addAttribute(Attribute *attribute, QString parent, Error *error)
{
    if(!alreadyExists(attribute->getName())){
        if(parent.isEmpty())
            atts.push_back(attribute);
        else{       //is complex attribute
            foreach(Attribute *att, atts)
                if(att->addAttribute(attribute, parent))
                    return true;
            return false;
        }
        return true;
    }
    else{
        error->setError(Error::NameTaken);
        return false;
    }
}

/**
 * Igual que el agregado, la suposicion de los atributos disponibles.
 */
bool Relationship::removeAttribute(QString attributesName)
{
    foreach(Attribute *att, atts){
            if(att->getName() == attributesName){
                atts.removeOne(att);
                return true;
            }
            else if(att->removeAttribute(attributesName))
                return true;
        }
    return false;
}

QList<Attribute *> Relationship::getAllAttributes()
{
    QList<Attribute*> tmp;
    foreach(Attribute* att, atts)
        tmp.append(att->getAttributes());
    return tmp;
}

QList<Attribute *> Relationship::getAttributes()
{
    QList<Attribute *> result;
    result.append(atts);
    return result;
}

/**
 * Lo mismo, si se aceptan otros tipos de atributos este metodo debe cambiar.
 */
QList<Attribute *> Relationship::getAttributesToSave()
{
    return getAttributes();
}

QList<Cardinality *> Relationship::getCardinalities()
{
    return this->cards;
}

QList<Entity *> Relationship::getEntities()
{
    return this->entities;
}

void Relationship::setEntities(QList<Entity *> entities)
{
    this->entities = entities;
}

bool Relationship::moveAttribute(QString attribute, int row, int newPos, QTableWidget *table)
{
    foreach(Attribute * att, atts){
        //when moving, skip whole attribute (if it's complex, it would be more than one row
        int index = atts.indexOf(att);
        int actualPos = newPos;
        if (row < newPos && index+1 < atts.size())   //downward movement
            actualPos = newPos + atts.at(index+1)->getAttributes().size() -1;
        else
            if (index-1 >= 0)
                actualPos = newPos - atts.at(index-1)->getAttributes().size() +1;

        if (att->getName() == attribute && att->moveAttribute(attribute,row,actualPos,table)){        //simple or comp parent attribute
            if (index >= 0){
                atts.removeOne(att);
                if (row < newPos && index+1 <= atts.size())     //move downward
                    atts.insert(index+1,att);
                else                                            //move upwards
                    if (index-1 >= 0)
                        atts.insert(index-1,att);
                    else atts.insert(index,att);                //if no movement is possible
                return true;
            }
        }
        if (att->getAttributes().size()>1 && att->moveAttribute(attribute,row,actualPos,table))       //attribute inside a complex one
            return true;
    }
    return false;
}

//given the entity name, return the option corresponding to it in such list
QString Relationship::getOption(Entity *entity, QList<QString> options)
{
    if (entities.size()==options.size()){
        if (entities.size()==2 && entities.at(0)==entities.at(1)) //unary rship
            return (cards.at(0)->getMax()=="1") ? options.at(0) : options.at(1);

        for(int i = 0; i < entities.size(); i++){
            if (entities.at(i) == entity)
                return options.at(i);
        }
    }
    return QString("");
}

QList<QString> Relationship::getOnDelete(){ return copyList(onDelete); }

void Relationship::setOnDelete(QList<QString> opts){ copyList(onDelete,opts); }

QList<QString> Relationship::getOnUpdate(){ return copyList(onUpdate); }

void Relationship::setOnUpdate(QList<QString> opts){ copyList(onUpdate,opts); }

QList<QString> Relationship::getMatch(){ return copyList(matching); }

void Relationship::setMatch(QList<QString> opts){ copyList(matching,opts); }

QDomElement *Relationship::getXML(QDomDocument *document, QString tagName)
{
    QDomElement * tmp = new QDomElement(document->createElement(tagName));
    tmp->setAttribute("name", this->getName());

    for(int i = 0; i< entities.size(); i++){
        tmp->setAttribute("name"+QString::number(i),((ERItem*)entities.at(i))->getName());
        tmp->setAttribute("onDelete"+QString::number(i),*onDelete.at(i));
        tmp->setAttribute("onUpdate"+QString::number(i),*onUpdate.at(i));
        tmp->setAttribute("match"+QString::number(i),*matching.at(i));
    }
    for (int j = 0; j < cards.size(); j++){
        tmp->setAttribute("min"+QString::number(j),cards.at(j)->getMin());
        tmp->setAttribute("max"+QString::number(j),cards.at(j)->getMax());
    }
    if (entities.size() < 3)    tmp->setAttribute("rolename",roleName);

    foreach(Attribute * att, getAttributesToSave())
        att->addToXML(document,tmp,QString(""));
    return tmp;
}

void Relationship::setRolename(QString rn)
{
    this->roleName = rn;
}

bool Relationship::alreadyExists(QString name){
    if(name.isEmpty())
        return false;
    foreach(Attribute *att, atts)
        if(att->hasAttribute(name))
            return true;
    return false;
}

QList<QString> Relationship::copyList(QList<QString *> list)
{
    QList<QString> result;
    foreach(QString*s,list)
        result << *s;
    return result;
}

void Relationship::copyList(QList<QString *> &list, QList<QString> opts)
{
    if (list.size()==opts.size())
        for (int i = 0; i < list.size(); i++){
            list.at(i)->clear();
            list.at(i)->append(opts.at(i));
        }
}
