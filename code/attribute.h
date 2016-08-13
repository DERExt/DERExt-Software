#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QString>
#include <QTableWidget>
#include <QtXml/QDomDocument>

#include <QDebug>

class Attribute
{
    bool in, multivalued, pk, sk, child;
    QString name;
    int optionSQL;

public:
    Attribute(QString attributeName, bool isNull, bool multivalued, bool primaryKey, bool secondaryKey, bool child){
        this->name = attributeName;
        this->child = child;
        if(child)
            this->in = false;
        else this->in = isNull;
        this->optionSQL = 1;
        this->pk = primaryKey;
        this->sk = secondaryKey;
        this->multivalued = multivalued;
    }

    QString getName() const{
        return this->name;
    }

    bool isNull() const{
        return this->in;
    }

    void setNull(bool null){
        if(!child)
            this->in = null;
    }

    void setName(QString name){
        this->name = name;
    }

    bool compare(Attribute *attribute){
        return name == attribute->getName();
    }

    bool isMultivalued() const{
        return this->multivalued;
    }

    bool isPrimaryKey() const{
        return this->pk;
    }

    bool isSecondaryKey() const{
        return this->sk;
    }

    bool isChild() const{
        return this->child;
    }

    void setPrimaryKey(bool primaryKey){
        this->pk = primaryKey;
    }

    void setSecondaryKey(bool secondaryKey){
        this->sk = secondaryKey;
    }

    void setMultivalued(bool multivalued){
        this->multivalued = multivalued;
    }

    void setOptionSQL(int a){
        this->optionSQL = a;
    }

    int getOptionSQL() const{
        return this->optionSQL;
    }

    void setChild(bool child){
        this->child = child;
        if(child)
            this->in = false;
    }

    bool equals(Attribute *a){
       return (this->getName() == a->getName());
    }

    bool moveRow(int row, int newPos, QTableWidget * table){
        if (row >= 0 && row <= table->rowCount()){
            QList<QTableWidgetItem*> items;
            if (newPos >= 0 && newPos <= table->rowCount()){
                items << table->takeItem(row,0) << table->takeItem(row,1) << table->takeItem(row,2);
                table->removeRow(row);
                table->insertRow(newPos);
                for (int i = 0; i < items.size(); i++)
                    table->setItem(newPos,i,items.at(i));
                table->setCurrentCell(newPos,0);
            }
            return true;
        }
        return false;
    }

    virtual QList<QString> getAttsNames()=0;
    virtual QString derive()=0;
    virtual void getDeriveList(QList<Attribute*> arr[])=0;
    virtual void addToTable(QTableWidget *table)=0;
    virtual bool addAttribute(Attribute *attribute, QString parent)=0;
    virtual bool removeAttribute(QString attributesName)=0;
    virtual bool hasAttribute(QString attributesName)=0;
    virtual QList<Attribute *> getChildren()=0;
    virtual bool canBePK()=0;
    virtual QList<Attribute*> getAttributes()=0;
    virtual int addToScene(QPainter *painter, int x, int y)=0;
    virtual void addToXML(QDomDocument *document, QDomElement *root, QString parent)=0;
    virtual Attribute *getCopy()=0;
    virtual void addToTableSQL(QTableWidget *table)=0;
    virtual void addToSQL(QString &tmp, QString namePrnt, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints, Attribute *primaryKey, Attribute *parent)=0;
    virtual QList<QString> getSQLName()=0;
    virtual bool moveAttribute(QString attribute,int row, int pos,QTableWidget * table)=0;
};

#endif // ATTRIBUTE_H

