#include "rbinary.h"

RBinary::RBinary(QString nameRship, Error *error, QList<Entity*> ents, QList<Cardinality*> cards)
    :Relationship(nameRship,error, ents, cards)
{
}

Relationship *RBinary::getCopy()
{
    QList<Entity*> newEnts;
    foreach(Entity * ent, entities)
        newEnts.append(ent->getCopy());
    QList<Cardinality*> newCards;
    foreach(Cardinality * c, cards){
        Cardinality * newCard = new Cardinality(c->getMin(), c->getMax());
        newCards.append(newCard);
    }

    RBinary * item = new RBinary(name,error,newEnts,newCards);
    foreach(Attribute * att, atts){
        Attribute * attC = att->getCopy();
        item->addAttribute(attC,QString(),error);
    }
    item->setRolename(this->roleName);
    return item;
}

QString RBinary::getSQL()
{
    QString tmp, final;
    bool unary = (entities.size()==2 && entities.at(0)->getName()==entities.at(1)->getName());
    if (cards.size() == 2 && cards.at(0)->getMax()!="1" && cards.at(1)->getMax()!="1"){      //N:N relationship
        QList<QString> tables, types, constraints;

        //Create table
        tmp.append("CREATE TABLE "+this->getName()+"\n(\n");
        QList<QString> pk;
        for (int i = 0; i < entities.size(); i++){
            QString res;
            addToSQL(tmp,i,tables,types,constraints);
            foreach(QString s, entities.at(i)->getPrimaryKey()->getSQLName())
                res+=s+",";
            res.chop(1);
            pk << res;
        }
        types.clear();  //otherwise, types already declared in entities are repeated

        //Primary key formation
        QString aux, fkRN;
        if (unary){ //unary relationship, atts have to have the rolename to avoid repetition in columns
            foreach(QString p, entities.at(0)->getPrimaryKey()->getSQLName())
                fkRN += roleName+"_"+p+",";
            fkRN.chop(1);
            aux = fkRN +","+ pk.at(0);
        } else {
            foreach(QString part, pk)   //pk in text
                aux+=part+",";
            aux.chop(1);
        }
        AttSimple * pkAtt = new AttSimple(aux,false,false,true,false,false);

        //Attributes
        foreach(Attribute *att, atts)
            att->addToSQL(tmp, this->getName(), tables, types, constraints, pkAtt, NULL);

        //Constraints and new tables
        tmp+="\n\tCONSTRAINT PK_"+this->getName()+" PRIMARY KEY ("+aux+"),";
        foreach(QString a, constraints)
            tmp.append(a);
        tmp.chop(1);
        tmp+="\n);";
        foreach(QString s, types)
            final.append(s+"\n\n");
        final.append(tmp+"\n\n");
        foreach(QString s, tables)
            final.append(s+"\n\n");
    }
    return final;
}

/**
 * @brief RBinary::addToSQL
 * This method is designed to be used from Entities, in N:1 relationships.
 */
void RBinary::addToSQL(QString &tmp, int ent, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints)
{
    bool isNN = (cards.at(0)->maxIsMultiple() && cards.at(1)->maxIsMultiple());
    bool isUnary = (entities.at(0)->getName()==entities.at(1)->getName());
    Entity * ent1 = entities.at(ent);
    Entity * entN = entities.at(abs(ent-1));
    QList<QString> noUse;

    //Primary Key + Attributes of relationship
    ent1->getPrimaryKey()->addToSQL(tmp,this->getName(),noUse,noUse,constraints,entN->getPrimaryKey(),NULL);
    addRolename(tmp,ent);
    if (!isNN){
        foreach(Attribute * att, getAttributes())
            att->addToSQL(tmp,this->getName(),tables,types,constraints,entN->getPrimaryKey(),NULL);
    }

    //Optionality of the entity
    QString pk_aux, pk_table;
    foreach(QString p, ent1->getPrimaryKey()->getSQLName()){
        if (isOptional(ent1->getName()) && !isNN){
            int index = tmp.indexOf(p);
            bool found=false;
            while(!found){
                if (tmp.mid(index,8)=="NOT NULL")
                    found=true;
                else index++;
            }
            tmp.remove(index,3);
        }
        if (!getRolename().isEmpty())
            pk_table+=getRolename()+"_"+p+",";
        pk_aux+=p+",";
    }
    pk_aux.chop(1);
    if (getRolename().isEmpty()
            || (isUnary && isNN && ent>0))
        pk_table = pk_aux;
    else pk_table.chop(1);

    //Alternate and foreign keys (constraints)
    QString constr;
    if (cards.at(0)->getMax()=="1" && cards.at(1)->getMax()=="1")       //1:1 rship
        constr+="\n\tCONSTRAINT AK_"+getName()+" UNIQUE "+"("+pk_table+"),";
    if (!getRolename().isEmpty() && (!isUnary || !isNN || ent==0))
        constr+="\n\tCONSTRAINT FK_"+ent1->getName()+"_"+getRolename();
    else constr+="\n\tCONSTRAINT FK_"+ent1->getName();
    constr+=" FOREIGN KEY ("+pk_table+")"
       +" REFERENCES "+ent1->getName()+"("+pk_aux+")"
       +" MATCH " + getMatch().at(ent).toLower()
       +" ON DELETE " + getOnDelete().at(ent).toLower()
       +" ON UPDATE " + getOnUpdate().at(ent).toLower() + ",";
    constraints << constr;

    //Table check
    Cardinality * nCard;    //cardinality of entityN
    if (isNN) tables << getAssertion(ent);
    else{
        nCard = (cards.at(0)->maxIsMultiple())?cards.at(0):cards.at(1);
        if (nCard->getMin()>=1 || nCard->getMax().toInt()!=0){ //table check is needed
            QString assert = "ALTER TABLE "+entN->getName()+"\nADD CONSTRAINT control_card_";
            if (!getRolename().isEmpty()) assert+=getRolename();
            assert+=entN->getName()+"\nCHECK ( NOT EXISTS ( SELECT 1 FROM "+entN->getName();
            if (((cards.at(0)==nCard)?cards.at(1):nCard)->getMin()!=0)
                assert+="\nWHERE "+ pk_table + " IS NOT NULL";
            assert+="\nGROUP BY "+ pk_table + "\nHAVING ";
            if (nCard->getMin()>=1) assert+="COUNT(*)<"+QString::number(nCard->getMin())+" OR ";
            if (nCard->getMax().toInt()!=0)
                assert+="COUNT(*)>"+nCard->getMax();
            else assert.chop(3);
            assert+=") );";
            tables << assert;
        }
    }
}

QString RBinary::getDerivation()
{
    QString aux("");
    if (!entities.at(0)->getPrimaryKey() || !entities.at(1)->getPrimaryKey())
        return aux;
    int entN = 0;
    bool isNN = (cards.at(0)->maxIsMultiple() && cards.at(1)->maxIsMultiple());

    //name + PKs
    if (isNN)
        aux = "NN_" + getName() + "[" + entities.at(0)->getPKder(false) + "," + entities.at(1)->getPKder(false) + ";";
    else{
        entN = (cards.at(0)->maxIsMultiple()) ? 0 : 1;
        if (cards.at(0)->maxIsMultiple() || cards.at(1)->maxIsMultiple())
            aux = "N1_" + getName() + "[" +entities.at(entN)->getPKder(false) + ";";
    }
    //AKs
    aux += ";";

    //Attributes
    QList<Attribute*> deriveAtts[4];
    foreach(Attribute * att,this->atts){
        att->getDeriveList(deriveAtts);
    }
    for (int i = Entity::secondaryKey + 1 ; i <= Entity::multAtts; i++){
        QString s;
        QList<Attribute*> a = deriveAtts[i];
        if(a.size()!=0){
            QList<Attribute*>::iterator it;
            for(it = a.begin() ; it!= a.end() ; it++)
                s+=(*it)->derive()+",";
            s.chop(1); //erase last comma
            aux.append(s);
        }
        aux.append(";");
    }

    //Cardinalities
    QString cardTxt;
    if (isNN || (!cards.at(0)->maxIsMultiple() && !cards.at(1)->maxIsMultiple()) //1:1 rship gets same treatment for cardinalities
            || (cards.at(0)->maxIsMultiple()))                                  //N:1 is the same if the first entity is the N one
        cardTxt = cards.at(0)->getText() + "," + cards.at(1)->getText() + ";";
    else
        if (cards.at(1)->maxIsMultiple())
            cardTxt = cards.at(1)->getText() + "," + cards.at(0)->getText() + ";";

    //FKs
    if (isNN)
        aux += cardTxt + entities.at(0)->getPKder() + "," + entities.at(1)->getPKder() + "]";
    else{
        if (!cards.at(0)->maxIsMultiple() && !cards.at(1)->maxIsMultiple()){      //1:1-> here we create both texts having a general structure
            //Create 1st derivation
            QString partial = aux;
            QString prep = "11_"+getName()+"_1["+entities.at(0)->getPKder()+";"+ entities.at(1)->getPKder(cards.at(1)->getMin()==0);
            aux.prepend(prep);
            aux += cardTxt + entities.at(1)->getPKder()+"]";

            //Create 2nd derivation
            aux+="\n11_" + getName() + "_2[" + entities.at(1)->getPKder() + ";" + entities.at(0)->getPKder(cards.at(0)->getMin()==0)
                    + partial + cards.at(1)->getText() + "," + cards.at(0)->getText() + ";" + entities.at(0)->getPKder() +"]";         //FK
        }
        else{
            int in = (entN==0) ? 1:0;
            aux += cardTxt + entities.at(in)->getPKder(cards.at(in)->getMin()==0) + "]";
        }
    }
    return aux;
}

void RBinary::addToTableSQL(QTableWidget *table)
{
    bool isNN = (cards.size()==2 && cards.at(0)->getMax()!="1" && cards.at(1)->getMax()!="1");
    bool is11 = (cards.size()==2 && cards.at(0)->getMax()=="1" && cards.at(1)->getMax()=="1");
    bool isUnary = (entities.at(0)->getName()==entities.at(1)->getName());
    for(int i = 0; i < entities.size(); i++){
        Entity * ent = entities.at(i);
        QString aux;
        if ((cards.at(i)->getMax()=="1" && !is11) ||        //N:1 rship (add just the 1 pk) or
            (isNN) ||                                       //N:N rship (add both PKs)
            (is11 && !ent->getFKs().contains(this)) ||      //1:1 add the one that DOESN'T have the relationship/AK
            (isUnary && is11)){                             //1:1 but Unary, must add only once.

            foreach(QString p, ent->getPrimaryKey()->getSQLName()){
                if ((entities.size()==2 && entities.at(0)->getName()==entities.at(1)->getName() && i==0) //unary rship
                    || (cards.at(i)->getMax()=="1" && !this->roleName.isEmpty()))
                    aux+=roleName+"_"+p+",";
                else aux+=p+",";
            }
            aux.chop(1);

            QTableWidgetItem * a = new QTableWidgetItem(aux);
            table->insertRow(table->rowCount());
            table->setItem(table->rowCount()-1,0,a);

            QList<QString> items;
            items << "Restrict" << "Cascade" << "No action";
            if (cards.size() == entities.size() && cards.at(i)->getMin()==0 && !isNN)
                items << "Set null";
            FKComboBox * cbOnDelete = new FKComboBox(table,onDelete.at(i));
            cbOnDelete->addOptions(items);
            table->setCellWidget(table->rowCount()-1,1,cbOnDelete);

            FKComboBox * cbOnUpdate = new FKComboBox(table,onUpdate.at(i));
            cbOnUpdate->addOptions(items);
            table->setCellWidget(table->rowCount()-1,2,cbOnUpdate);
            items.clear();
            items << "Simple" << "Partial" << "Full";
            FKComboBox * cbMatching = new FKComboBox(table,matching.at(i));
            cbMatching->addOptions(items);
            table->setCellWidget(table->rowCount()-1,3,cbMatching);

            if (isUnary && is11) return;
        }
    }
}

QList<Relationship *> RBinary::getFKs()
{
    QList<Relationship*> result;
    if(cards.size()==2 && cards.at(0)->getMax()!="1" && cards.at(1)->getMax()!="1") //N:N rship
        result << this;
    return result;
}

QDomElement *RBinary::getXML(QDomDocument *document)
{
    return Relationship::getXML(document,"RBinary");
}

QString RBinary::getAssertion(int ent)
{
    QString final;
    if (ent>=0 && ent<cards.size() && (cards.at(ent)->getMin()>=1 || cards.at(ent)->getMax().toInt()!=0)){
        final+="\nCREATE ASSERTION as_lim_"+this->getName()+"_"+entities.at(ent)->getName()
                +"\nCHECK ( NOT EXISTS ( SELECT 1 FROM "+getName()+" t1 JOIN " + entities.at(abs(ent-1))->getName() +" t2 ON ";
        QList<QString> pkTable = entities.at(abs(ent-1))->getPrimaryKey()->getSQLName();
        QString aux;
        for (int i = 0 ; i < pkTable.size(); i++){
            final+="t1.";
            if (!getRolename().isEmpty()) final+=getRolename()+"_";
            final+=pkTable[i]+"=t2."+pkTable[i]+" AND \n";
            aux+=pkTable[i]+",";
        }
        final.chop(6);
        aux.chop(1);
        final+="\nGROUP BY "+aux+"\nHAVING ";
        if (cards.at(ent)->getMin()>=1)
            final+="COUNT(*)<"+QString::number(cards.at(ent)->getMin())+" OR";
        if (cards.at(ent)->getMax().toInt()!=0)
            final+=" COUNT(*)>"+cards.at(ent)->getMax();
        else final.chop(2);
        final+=") );";
    }
    return final;
}

void RBinary::addRolename(QString & tmp, int ent)
{
    if (entities.at(0)->getName()==entities.at(1)->getName() &&
            cards.at(0)->maxIsMultiple() && cards.at(1)->maxIsMultiple() && ent>0) return;

    if (!getRolename().isEmpty()){
        foreach(QString p, entities.at(ent)->getPrimaryKey()->getSQLName()){
            int index = tmp.indexOf(QRegExp("\t"+p));
            if (index > 0){
                int i = index+1;
                int tam = 0;
                while (i < tmp.size() && tmp[i++]!=' ') tam++;
                tmp.replace(index+1,tam,getRolename()+"_"+p);
            }
        }
    }
}

bool RBinary::isOptional(QString entityName)
{
    QList<ERItem*> ents;
    foreach(Entity * e, getEntities())
        ents.append(e);

    for(int i = 0; i < ents.size(); i++)
        if (ents.at(i)->getName()==entityName && cards.at(i)->getMin()==0)
            return true;
    return false;
}

