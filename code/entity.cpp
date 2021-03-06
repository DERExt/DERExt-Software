#include "entity.h"
#include "hierarchy.h"
#include <QDebug>

Entity::Entity(QString name, Error * error, bool isAssociation, Relationship * rel)
{
    this->name = name;
    this->primaryKey = NULL;
    this->error = error;
    this->altKey = 1;
    this->fatherH = NULL;
    this->sonH =NULL;
    this->newInDERE = true;
    this->associationRel = rel;
    this->isAssociation = isAssociation;
    subtype = false;
}

QString Entity::getName()
{
    return this->name;
}

void Entity::setName(QString name)
{
    this->name = name;
}

QString Entity::getPrefix()
{
    if (!this->isAssociative()) {
        if(sonH != NULL && fatherH != NULL)
            return "B_";
        else
            if(sonH != NULL && fatherH == NULL)
                return "S_";
            else
                if(sonH == NULL && fatherH != NULL)
                    return "P_";
                else
                    return "R_";
    }
    else {
        //Asociación.
        return "A_";
    }
}

QString Entity::getDerivation()
{
    QString aux("");
    QString prefix = getPrefix();
    QList<Attribute*> derArr[4];
    foreach(Attribute* att, secondaryKeys)
        derArr[secondaryKey].append(att);
    foreach(Attribute* att, atts)
        att->getDeriveList(derArr);
    if(getPrimaryKey()!=NULL){
        aux = prefix+this->getName()+"["+getPrimaryKey()->derive()+";";
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
        if(fatherH != NULL){
            QString type = fatherH->getTypeName();
            if(fatherH->isTotal())
                aux.append("(T,"+type+")]");
            else
                aux.append("(P,"+type+")]");
        }
        else
            aux.append("]");
    }
    else error->setError(Error::NoPrimaryKey);
    return aux;
}

QString Entity::getSQL()
{
    QList<QString> tables, types, constraints, noUse;
    QString tmp, final;
    QList<Entity*> fkEntities;

    if(primaryKey!=NULL){
        tmp.append("CREATE TABLE "+this->getName()+"\n(\n");
        primaryKey->addToSQL(tmp, this->getName(), tables, types, constraints, primaryKey, NULL);
        foreach(Attribute *att, secondaryKeys)
            att->addToSQL(tmp, this->getName(), tables, types, constraints, primaryKey, NULL);
        foreach(Attribute *att, atts)
            att->addToSQL(tmp, this->getName(), tables, types, constraints, primaryKey, NULL);
        foreach(Relationship * rship, N1rships){
            int ent = (rship->getCardinalities().at(0)->getMax()=="1") ? 0 : 1;
            ((RBinary*)rship)->addToSQL(tmp, ent, tables, types, constraints);
        }
        QString aux;
        foreach(QString s, primaryKey->getSQLName())
            aux += s+", ";
        aux.chop(2);
        tmp.append("\n\tCONSTRAINT PK_"+this->getName()+primaryKey->getName()+" PRIMARY KEY ("+aux+"),");
        foreach(QString a, constraints)
            tmp.append(a);
        tmp.chop(1);
        tmp.append("\n);");
    }
    else error->setError(Error::NoPrimaryKey);
    foreach(QString s, types)
        final.append(s+"\n\n");
    final.append(tmp+"\n\n");
    foreach(QString s, tables)
        final.append(s+"\n\n");
    return final;
}

bool Entity::moveAttribute(QString attribute, int row, int newPos, QTableWidget *table)
{
    foreach(Attribute * att, this->atts){
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
                Attribute * aux = att;
                atts.removeOne(att);
                if (row < newPos && index+1 <= atts.size())     //move downward
                    atts.insert(index+1,aux);
                else                                            //move upwards
                    if (index-1 >= 0)
                        atts.insert(index-1,aux);
                    else atts.insert(index,aux);                //if no movement is possible
                return true;
            }
        }
        if (att->getAttributes().size()>1 && att->moveAttribute(attribute,row,actualPos,table))       //attribute inside a complex one
            return true;
    }
    return false;
}

void Entity::addN1Rship(Relationship *rship)
{
    this->N1rships.append(rship);
}

Attribute *Entity::getPrimaryKey() const
{
    if((fatherH != NULL && sonH == NULL) || (fatherH == NULL && sonH == NULL))
        return this->primaryKey;
    if(sonH != NULL)/*Arreglar*/
        return sonH->getPrimaryKey();
}

QString Entity::getPKder(bool isOpt)
{
    QString result = getName() + "." ;
    if (isOpt) result+=QChar(0x00AC);
    return result+getPrimaryKey()->derive();
}

bool Entity::setPrimaryKey(Attribute *pk)
{
    if(subtype){
        return this->setSecondaryKey(pk);
    }else{
    if(pk == NULL){
        this->removeAttribute(pk->getName());
        this->primaryKey = NULL;
        pk->setPrimaryKey(false);
        return true;
    }else{
        if(pk->canBePK() && !isChild(pk) && !compoundSecondary.contains(pk)){
            this->removeAttribute(pk->getName());
            if(this->primaryKey!=NULL){
                Attribute *tmp = this->primaryKey;
                this->primaryKey = pk;
                pk->setSecondaryKey(false); //in case it was SK
                tmp->setPrimaryKey(false);
                this->addAttribute(tmp, QString(""), error);
            }
            else this->primaryKey = pk;
            pk->setPrimaryKey(true);
            return true;
        }
        else
            return false;
    }
}
}

bool Entity::setSecondaryKey(Attribute *sk)
{
    if(sk!=primaryKey){
        if(!isChild(sk)){
            this->removeAttribute(sk->getName());
            secondaryKeys.append(sk);
            sk->setSecondaryKey(true);
            return true;
        }
        else{
            error->setError(Error::SecondaryChild);
            return false;
        }
    }
    else {
        error->setError(Error::PrimarySecondary);
        return false;
    }
}

bool Entity::removePrimaryKey(Attribute *pk)
{
    primaryKey = NULL;
    pk->setPrimaryKey(false);
    return true;
}

bool Entity::setSecondaryKeyComp(QList<Attribute *> atts)
{
    if(!atts.isEmpty()){
        foreach(Attribute *att, atts)
            this->removeSecondaryKey(att->getName());
        foreach(Attribute *att, atts){
            if(!att->isChild())
                this->removeAttribute(att->getName());
        }
        QString name("altKey"+QString::number(altKey));
        Attribute *aux = new AttComp(name, false, atts.size(), false, false, true, false);
        altKey++;
        foreach(Attribute *att, atts)
            aux->addAttribute(att, name);
        this->setSecondaryKey(aux);
        compoundSecondary.append(aux);
        return true;
    }
    return false;
}

bool Entity::removeSecondaryKey(QString sk)
{
    foreach(Attribute *att, secondaryKeys){
        if(att->getName() == sk){
            att->setSecondaryKey(false);
            secondaryKeys.removeOne(att);
            if(compoundSecondary.contains(att)){
                QList<Attribute *> tmp = ((AttComp*)att)->getAllAttributes();
                foreach(Attribute *a, tmp){
                    if(!a->isChild())
                        this->addAttribute(a, QString(""), error);
                }
                compoundSecondary.removeAll(att);
                altKey--;
            }
            else this->addAttribute(att, QString(""), error);
            return true;
        }
    }
    return false;
}

bool Entity::isChild(Attribute *attribute)
{
    QList<Attribute *> children;
    if(primaryKey!=NULL)
        children.append(primaryKey->getChildren());
    foreach(Attribute *att, secondaryKeys)
        children.append(att->getChildren());
    foreach(Attribute *att, atts)
        children.append(att->getChildren());
    foreach(Attribute *att, children)
        if(att->equals(attribute))
            return true;
    return false;
}

/**
 * @brief Entity::getRship
 * returns N:1 relationship in which the current entity
 * is connected to ent.
 */
Relationship *Entity::getRship(Entity *ent)
{
    foreach(Relationship * r, N1rships)
        if (r->getEntities().size()>=2){
            if (r->getEntities().at(0)->getName()==ent->getName())
                return r;
            else
                if (r->getEntities().at(1)->getName()==ent->getName())
                    return r;
        }
    return NULL;
}

bool Entity::alreadyExists(QString name){
    if(name.isEmpty())
        return false;
    if(primaryKey!=NULL && primaryKey->hasAttribute(name))
        return true;
    foreach(Attribute *att, secondaryKeys)
        if(att->hasAttribute(name))
            return true;
    foreach(Attribute *att, atts)
        if(att->hasAttribute(name))
            return true;
    return false;
}

void Entity::setOldState(bool state){
    newInDERE = state;
}

bool Entity::addAttribute(Attribute *attribute, QString parent, Error *error)
{
    if(!alreadyExists(attribute->getName())){
        if(parent.isEmpty()){
            /*if(!subtype && newInDERE){
                if((primaryKey!=NULL || !setPrimaryKey(attribute)))
                    atts.push_back(attribute);
            }else{*/
                atts.push_back(attribute);
                return true;
            /*}
            newInDERE = false;*/
        }
        else{ //is complex attribute
            /*if((!subtype && newInDERE && this->primaryKey!=NULL && this->primaryKey->addAttribute(attribute, parent))){
                newInDERE = false;
                return true;
            }else{*/
                /*foreach(Attribute *att, secondaryKeys){
                    if(att->addAttribute(attribute, parent))
                        return true;
                }*/
                foreach(Attribute *att, atts){
                    if(att->addAttribute(attribute, parent))
                        return true;
                }
                return false;
            //}
        }
        return true;
    }
    else{
        error->setError(Error::NameTaken);
        return false;
    }
}


bool Entity::removeAttribute(QString attributesName)
{
    if(this->primaryKey!=NULL){
        if(this->primaryKey->getName() == attributesName){
            this->primaryKey = NULL;
            return true;
        }
        else if(primaryKey->removeAttribute(attributesName))
            return true;
    }
    foreach(Attribute *att, secondaryKeys){
        if(att->getName() == attributesName){
            secondaryKeys.removeOne(att);
            return true;
        }
        else if(att->removeAttribute(attributesName))
            return true;
    }
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

QList<Attribute *> Entity::getAllAttributes()
{
    QList<Attribute*> tmp;
    if(primaryKey!=NULL)
        tmp.append(primaryKey->getAttributes());
    foreach(Attribute* att, secondaryKeys)
        tmp.append(att->getAttributes());
    foreach(Attribute* att, atts)
        tmp.append(att->getAttributes());
    return tmp;
}

QList<Attribute *> Entity::getAttributes()
{
    QList<Attribute*> tmp;
    if(primaryKey!=NULL)/***************************/
        tmp.append(primaryKey);
    tmp.append(secondaryKeys);
    tmp.append(atts);
    return tmp;
}

QList<QString> Entity::getSKToSave()
{
    QList<Attribute*> tmp;
    QList<QString> aux;
    foreach(Attribute *att, compoundSecondary)
        tmp.append(((AttComp *)att)->getAllAttributes());
    foreach(Attribute *att, tmp)
            aux.append(att->getName());
    return aux;
}

void Entity::addCompSK(QString name)
{
    foreach(Attribute *att, this->getAllAttributes())
        if(att->getName() == name)
            partialSK.append(att);
}

void Entity::generateCompSK()
{
    this->setSecondaryKeyComp(partialSK);
}

void Entity::setAssociation(Relationship * r) {
    this->isAssociation = true;
    this->associationRel = r;
}

Relationship * Entity::getAssociationRel() {
    return this->associationRel;
}

bool Entity::isAssociative() {
    return this->isAssociation;
}

Hierarchy* Entity::getHierarchy()
{/***************************************************/
    return fatherH;
}

void Entity::setHierarchy(Hierarchy* item)
{/***************************************************/
    fatherH = item;
}

bool Entity::isFather() const
{/***************************************************/
    if(fatherH != NULL)
        return true;
    return false;
}
void Entity::RemoveHierarchy(){
    fatherH = NULL;
}

void Entity::RemoveHierarchySon()
{/***************************************************/
    sonH=NULL;
}

void Entity::setHierarchySon(Hierarchy* item)
{/***************************************************/
    sonH = item;
}

Hierarchy *Entity::getHierarchySon()
{/***************************************************/
    return sonH;
}

QList<Attribute *> Entity::getAttributesToSave()
{
    QList<Attribute*> tmp = this->getAttributes(), aux;
    foreach(Attribute *att, compoundSecondary)
        tmp.removeAll(att);
    foreach(Attribute *att, compoundSecondary)
        aux.append(((AttComp *)att)->getAllAttributes());
    foreach(Attribute *att, aux)
        if(!att->isChild())
            tmp.append(att);
    return tmp;
}

QList<Attribute *> Entity::getSecondaryKeys()
{
    return this->secondaryKeys;
}

Entity *Entity::getCopy()
{
    Entity *tmp = new Entity(this->name, this->error);
    tmp->setSubtype(subtype);
    tmp->setHierarchy(fatherH);
    tmp->setHierarchySon(sonH);
    tmp->setOldState(newInDERE);/***** NEW *****/
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

QList<Relationship *> Entity::getFKs()
{
    return this->N1rships;
}

QDomElement *Entity::getXML(QDomDocument *document) {
    QDomElement *tmp = new QDomElement(document->createElement("Entity"));
    tmp->setAttribute("name", this->getName());
    tmp->setAttribute("subtype",subtype);

    tmp->setAttribute("isAssociative", this->isAssociative());
    if(this->isAssociative())
        tmp->setAttribute("associationRel", this->associationRel->getName());
    foreach(Attribute *att, getAttributesToSave())
        att->addToXML(document, tmp, QString("")); //second argument is parent

    return tmp;
}

bool Entity::isSubtype()
{
    return this->subtype;
}

void Entity::setSubtype(bool op)
{
    this->subtype = op;
}
