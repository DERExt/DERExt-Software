#include "weakentity.h"

WeakEntity::WeakEntity(QString entityName, Error *error, Entity *strongEntity):
    Entity(entityName,error)
{
    this->strongEntity = strongEntity;
}

bool WeakEntity::addAttribute(Attribute *attribute, QString parent, Error *error)
{
    if (strongEntity!=NULL && strongEntity->getPrimaryKey()!=NULL)
        foreach(Attribute * att, strongEntity->getPrimaryKey()->getAttributes())
            if (att->getName()==attribute->getName()){
                error->setError(Error::NameTaken);
                return false;
            }
    return Entity::addAttribute(attribute,parent,error);
}

QString WeakEntity::getDerivation()
{
    QString aux("");
    QList<Attribute*> derArr[4];
    foreach(Attribute* att, secondaryKeys)
        derArr[secondaryKey].append(att);
    foreach(Attribute* att, atts)
        att->getDeriveList(derArr);
    if(primaryKey!=NULL && strongEntity->getPrimaryKey()!=NULL){
        aux = "D_"+this->getName()+"[";
        aux += strongEntity->getPrimaryKey()->derive()+","+primaryKey->derive()+";";
        for(int i = secondaryKey ; i<=multAtts ; i++){
            QString s;
            QList<Attribute*> a = derArr[i];
            if(a.size()!=0){
                QList<Attribute*>::iterator it;
                for(it = a.begin() ; it!= a.end() ; it++)
                    s+=(*it)->derive()+",";
                s.chop(1); //erase last comma
                aux.append(s);
            }
            aux.append(";");
        }
        aux.append("]");
    }
    else error->setError(Error::NoPrimaryKey);
    return aux;
}

Entity *WeakEntity::getCopy()
{
    Entity * tmp = new WeakEntity(this->name, this->error, this->strongEntity->getCopy());
    if(primaryKey!=NULL){
        Attribute *aux = primaryKey->getCopy();
        tmp->addAttribute(aux, "", this->error);
        tmp->setPrimaryKey(aux);
    }
    foreach(Attribute* att, secondaryKeys){
        Attribute *aux = att->getCopy();
        tmp->addAttribute(aux, "", this->error);
        tmp->setSecondaryKey(aux);
    }
    foreach(Attribute* att, atts)
        tmp->addAttribute(att->getCopy(), "", this->error);
    return tmp;
}

Entity * WeakEntity::getStrongEntity(){
    return this->strongEntity;
}

QString WeakEntity::getSQL()
{
    QString tmp = Entity::getSQL();
    int index = tmp.indexOf(" PRIMARY KEY (");
    index = index + 14;
    while (tmp[index]!=')')
        index++;
    QString aux;
    foreach(QString pk, strongEntity->getPrimaryKey()->getSQLName())
        aux+=pk+",";
    aux.chop(1);
    tmp.insert(index,","+aux);

    return tmp;
}

QDomElement *WeakEntity::getXML(QDomDocument *document)
{
    QDomElement *tmp = new QDomElement(document->createElement("Entity"));
    tmp->setAttribute("name", this->getName());
    tmp->setAttribute("strongEnt",this->strongEntity->getName());
    tmp->setAttribute("weak",true);

    foreach(Attribute *att, getAttributesToSave())
        att->addToXML(document, tmp, QString("")); //second argument is parent
    return tmp;
}
