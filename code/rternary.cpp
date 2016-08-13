#include "rternary.h"

RTernary::RTernary(QString nameRship, Error *error, QList<Entity *> ents, QList<Cardinality *> cards):
    Relationship(nameRship,error,ents,cards)
{
}

Relationship *RTernary::getCopy()
{
    QList<Entity*> newEnts;
    newEnts.append(entities);
    QList<Cardinality*> newCards;
    foreach(Cardinality * c, cards){
        Cardinality * newCard = new Cardinality(c->getMin(), c->getMax());
        newCards.append(newCard);
    }

    RTernary * item = new RTernary(name,error,newEnts,newCards);
    foreach(Attribute * att, atts){
        Attribute * attC = att->getCopy();
        item->addAttribute(attC,QString(),error);
    }
    item->setRolename(roleName);
    return item;
}

QString RTernary::getSQL()
{
    QString tmp, final;
    QList<QString> tables, types, constraints;

    tmp.append("CREATE TABLE "+this->getName()+"\n(\n");
    QList<QString> pks = getPKs(false);
    QString pk;
    if (pks.size() > 0) pk = pks.at(0);
    foreach(Entity * ent, entities)
        ent->getPrimaryKey()->addToSQL(tmp, this->getName(), tables, types, constraints, ent->getPrimaryKey(), NULL);
    types.clear();  //otherwise, types already declared in entities are repeated

    //Optionality in N:N:1 relationship
    if (pks.size()==1 && (!cards[0]->maxIsMultiple() || !cards[1]->maxIsMultiple() || !cards[2]->maxIsMultiple())){
        int index = (!cards[0]->maxIsMultiple()) ? 0 : 1;
        index = (!cards[2]->maxIsMultiple()) ? 2 : index;
        if (cards[index]->getMin()==0){
            foreach(QString p, entities[index]->getPrimaryKey()->getSQLName()){
                int index = tmp.indexOf(p);
                bool found=false;
                while(!found){
                    if (tmp.mid(index,8)=="NOT NULL")
                        found=true;
                    else index++;
                }
                tmp.remove(index,3);
            }
        }
    }

    AttSimple * pkAtt = new AttSimple(pk,false,false,true,false,false);
    foreach(Attribute *att, atts)
        att->addToSQL(tmp, this->getName(), tables, types, constraints, pkAtt, NULL);
    foreach(QString a, constraints)
        tmp.append(a);

    tmp.append("\n\tCONSTRAINT PK_"+this->getName()+" PRIMARY KEY ("+pk+"),");
    if (!pks.isEmpty()) pks.pop_front();
    foreach(QString p, pks)
        tmp.append("\n\tCONSTRAINT AK"+QString::number(pks.indexOf(p))+"_"+getName()+" UNIQUE ("+p+")"+",");
    foreach(Entity * e, entities)
        tmp.append("\n\tCONSTRAINT FK_"+e->getName()+" FOREIGN KEY ("+joinPK(e,false)+
                   ") REFERENCES "+e->getName()+"("+joinPK(e,false)+")"
                   +" MATCH " + getOption(e,getMatch()).toLower()
                   +" ON DELETE " + getOption(e,getOnDelete()).toLower()
                   +" ON UPDATE " + getOption(e,getOnUpdate()).toLower() +","
                   );
    tmp.chop(1);
    tmp.append("\n);");

    foreach(QString s, types)
        final.append(s+"\n\n");
    final.append(tmp+"\n\n");
    foreach(QString s, tables)
        final.append(s+"\n\n");
    final+=getAssertions();
    return final;
}

QString RTernary::getDerivation()
{
    QString result;
    QList<QString> pks = getPKs(true);
    if (pks.isEmpty())
        return result;

    //name + PK
    if (pks.size()==1)      //N:N:N or N:N:1
        if (cards.at(0)->maxIsMultiple() && cards.at(1)->maxIsMultiple() && cards.at(2)->maxIsMultiple()) //N:N:N
            result = "NNN_";
        else result = "NN1_";
    else{
        if (pks.size()==0) return QString("");
        else{
            if (!cards.at(0)->maxIsMultiple() && !cards.at(1)->maxIsMultiple() && !cards.at(2)->maxIsMultiple()) //1:1:1
                result = "111_";
            else
                result = "N11_";
        }
    }
    result += getName() + "[" + pks.at(0) + ";";
    pks.pop_front();

    //AKs
    foreach(QString ak, pks){
        result+=ak+"-";
    } if (pks.size() > 0) result.chop(1);
    result+=";";

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
            result.append(s);
        }
        result.append(";");
    }

    //Cardinalitites
    int ent1=-1;
    int entN=-1;
    int n1, n2;
    if ((cards.at(0)->getMax()!="1" && cards.at(1)->getMax()!="1" && cards.at(2)->getMax()!="1")        //N:N:N
           || (cards.at(0)->getMax()=="1" && cards.at(1)->getMax()=="1" && cards.at(2)->getMax()=="1"))   //1:1:1
        result+= cards.at(0)->getText() + "," +cards.at(1)->getText() + "," + cards.at(2)->getText() + ";";
    else{                   //N:N:1 or N:1:1, cardinalities must be according to the PK selected
        for (int i = 0; i < cards.size(); i++)      //see if it's a N:N:1
            if (cards[i]->getMax()=="1"){
                if (ent1==-1) ent1 = i;
                else ent1 = -1; }
        if (ent1 <0)                                //it's an N:1:1
            for (int i = 0; i < cards.size(); i++)
                if (cards[i]->getMax()!="1")
                    entN = i;

        int c = (ent1<0)?entN:ent1;
        getOthersIndex(c,n1,n2);
        QString parcCard = cards.at(n1)->getText() + "," + cards.at(n2)->getText();
        if (ent1<0)
            result+= cards.at(entN)->getText() + "," + parcCard + ";";
        else result+= parcCard + "," + cards.at(ent1)->getText() + ";";
    }

    //FKs
    if (ent1<0 && entN<0)       //1:1:1 or N:N:N
        result+= entities.at(0)->getPKder() + "," + entities.at(1)->getPKder() + "," + entities.at(2)->getPKder() + "]";
    else{
        QString parcFk = entities.at(n1)->getPKder() + "," + entities.at(n2)->getPKder();
        if (ent1<0)
            result+= entities.at(entN)->getPKder() +"," +parcFk + "]";
        else result+= parcFk + "," + entities.at(ent1)->getPKder() + "]";
    }
    return result;
}

void RTernary::addToTableSQL(QTableWidget *table)
{
    for(int i = 0; i < entities.size(); i++){
        Entity * ent = entities.at(i);
        QString aux;
        foreach(QString p, ent->getPrimaryKey()->getSQLName())
            aux+=p+",";
        aux.chop(1);

        QTableWidgetItem * a = new QTableWidgetItem(aux);
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1,0,a);

        QList<QString> items;
        items << "Restrict" << "Cascade" << "No action";
        if (cards.size() == entities.size() && cards.at(i)->getMin()==0
                && !cards.at(i)->maxIsMultiple() && getPKs(false).size()==1)     //N:N:1, the 1 entity gets the Set null option
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
    }
}

QList<Relationship *> RTernary::getFKs()
{
    QList<Relationship*> result;
    result << this;
    return result;
}

QDomElement *RTernary::getXML(QDomDocument *document)
{
    return Relationship::getXML(document,"RTernary");
}

QList<QString> RTernary::getPKs(bool derivELE)
{
    QList<QString> pks;
    if (!entities.at(0)->getPrimaryKey() || !entities.at(1)->getPrimaryKey() || !entities.at(2)->getPrimaryKey())
        return pks;
    if (cards.size()==3){
        QString max1 = cards.at(0)->getMax();
        QString max2 = cards.at(1)->getMax();
        QString max3 = cards.at(2)->getMax();

        if (max1!="1" && max2!="1" && max3!="1")        //N:N:N
            pks << joinPK(entities.at(0),derivELE)+","+joinPK(entities.at(1),derivELE)+","+joinPK(entities.at(2),derivELE);
        else{
            if (max1=="1" && max2=="1" && max3=="1"){    //1:1:1
                pks << joinPK(entities.at(0),derivELE)+","+joinPK(entities.at(1),derivELE);
                pks << joinPK(entities.at(0),derivELE)+","+joinPK(entities.at(2),derivELE);
                pks << joinPK(entities.at(1),derivELE)+","+joinPK(entities.at(2),derivELE);
            }
            else{
                Entity *e1, *e1p, *eN, *eNp;
                if ((max1=="1" && max2=="1") || (max2=="1" && max3=="1") || (max1=="1"&&max3=="1")){    //1:1:N
                    e1 = (max1=="1")?entities.at(0):entities.at(1);
                    e1p = (max3=="1")?entities.at(2):entities.at(1);
                    eN = (max1!="1")?entities.at(0):entities.at(1);
                    eNp = NULL;
                    if (eN==entities.at(1)) eN = (max2!="1")?entities.at(1):entities.at(2);
                }
                else{                                //1:N:N
                    e1 = (max1=="1")?entities.at(0):entities.at(1);
                    if (e1==entities.at(1)) e1 = (max3=="1")?entities.at(1):entities.at(2);
                    eN = (max1!="1")?entities.at(0):entities.at(1);
                    eNp = (max3!="1")?entities.at(2):entities.at(1);
                }

                if (eN!=NULL && eNp!=NULL)         //1:N:N
                    pks << joinPK(eN,derivELE) + "," + joinPK(eNp,derivELE);
                else{                               //1:1:N
                    if (e1!=NULL && e1p!=NULL){
                        pks << joinPK(eN,derivELE) + "," + joinPK(e1,derivELE);
                        pks << joinPK(eN,derivELE) + "," + joinPK(e1p,derivELE);
                    }
                }
            }
        }
    }
    return pks;
}

QString RTernary::joinPK(Entity *ent, bool derivELE)
{
    if (derivELE)
        return QString(ent->getPKder());

    QString aux;
    foreach(QString s, ent->getPrimaryKey()->getSQLName())
        aux+=s+",";
    aux.chop(1);
    return aux;
}

/**
 * @brief RTernary::getAssertions
 * Depending on the type of ternary rship, return assertions to be inserted at the end
 */
QString RTernary::getAssertions()
{
    QString result;
    for (int i = 0; i < cards.size(); i++)
        if (cards.at(i)->maxIsMultiple())
            result+=getAssertion(i)+"\n";
    return result;
}

QString RTernary::getAssertion(int ent){
    if (cards.at(ent)->getMin()>=1 || cards.at(ent)->getMax().toInt()!=0){
        QString result, aux;
        int ent1, ent2;
        getOthersIndex(ent, ent1, ent2);
        result = "\nCREATE ASSERTION as_lim_"+getName()+"_"+entities.at(ent)->getName()
            + "\nCHECK ( NOT EXISTS ( SELECT 1 FROM " + getName()+" t1";
        //JOIN Tables
        QList<int> ents;
        ents << ent1 << ent2;
        for (int i = 0; i < ents.size(); i++){
            result+="\nJOIN " + entities.at(ents.at(i))->getName() + " t"+QString::number(i+2)+" ON";
            foreach(QString att, entities.at(ents.at(i))->getPrimaryKey()->getSQLName()){
                aux+=att+",";
                result+=" t1."+att+"=t"+QString::number(i+2)+"."+att+" AND \n";
            }
            result.chop(6);
        }
        aux.chop(1);
        result+="\nGROUP BY "+aux+"\nHAVING ";
        if (cards.at(ent)->getMin()>0) result+="COUNT(*)<"+QString::number(cards.at(ent)->getMin())+" OR";
        if (cards.at(ent)->getMax().toInt()!=0) result += " COUNT(*)>"+cards.at(ent)->getMax();
        else result.chop(2);
        result+="));";

        return result;
    }
}

void RTernary::getOthersIndex(int selected, int &o1, int &o2)
{
    switch(selected){
    case 0: o1=1; o2=2; break;
    case 1: o1=0; o2=2; break;
    case 2: o1=0; o2=1; break;
    }
}
