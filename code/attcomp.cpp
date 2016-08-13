#include "attcomp.h"
#include "addpushbutton.h"
#include "attdialog.h"

#include <QPushButton>
#include <QPainter>

AttComp::AttComp(QString n, bool isNull, int amountAttributes, bool multivalued, bool primaryKey, bool secondaryKey, bool child):Attribute(n, isNull, multivalued, primaryKey, secondaryKey, child)
{
    this->amountAttributes = amountAttributes;
}

QList<QString> AttComp::getAttsNames()
{
    QList<QString> a;
    foreach(Attribute *att, atts)
        a.push_back(att->getName());
    return a;
}

QString AttComp::derive()
{
    QString names, tmp;
    foreach(Attribute *att, atts)
        names.append(att->derive()+",");
    names.chop(1);
    tmp.append(this->getName()+"("+names+")");
    if(this->isMultivalued()){
        tmp.push_front("{");
        tmp.append("}");
    }
    if(this->isNull())
        tmp.push_front(QChar(0x00AC));
    return tmp;
}

void AttComp::getDeriveList(QList<Attribute *> arr[])
{
    if(this->isMultivalued())
        arr[Entity::multAtts].push_back(this);
    else arr[Entity::compAtts].push_back(this);
}

bool AttComp::addAttribute(Attribute *attribute, QString parent)
{
    if(this->getName() == parent){
        atts.push_back(attribute);
        return true;
    }
    else {
        foreach(Attribute *att, atts){
            if(att->addAttribute(attribute, parent))
                return true;
        }
        return false;
    }
}

bool AttComp::removeAttribute(QString attributesName)
{
    foreach(Attribute *att, atts){
        if(att->getName() == attributesName){
            atts.removeOne(att);
            return true;
        }
        else if (att->removeAttribute(attributesName))
            return true;
    }
    return false;
}

bool AttComp::hasAttribute(QString attributesName)
{
    if(this->getName() == attributesName)
        return true;
    foreach(Attribute *att, atts){
        if(att->hasAttribute(attributesName))
            return true;
    }
    return false;
}

QList<Attribute *> AttComp::getChildren()
{
    QList<Attribute *> children;
    children.append(atts);
    foreach(Attribute *att, atts)
        children.append(att->getChildren());
    return children;
}

bool AttComp::canBePK()
{
    return !(this->isNull() || this->isMultivalued());
}

QList<Attribute *> AttComp::getAttributes()
{
    QList<Attribute*> tmp;
    tmp.append(this);
    foreach(Attribute *att, atts)
        tmp.append(att->getAttributes());
    return tmp;
}

QList<Attribute *> AttComp::getAllAttributes()
{
    return this->atts;
}

int AttComp::addToScene(QPainter *painter, int x, int y)
{
    QPen pen = painter->pen();

    if(this->isMultivalued()){
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawLine(QPoint(x+20, y+12), QPoint(x+15, y+7));
        painter->drawLine(QPoint(x+20, y+12), QPoint(x+15, y+17));
        painter->setPen(pen);
    }
    int dy = y;
    painter->drawText(QRectF(QPointF(x+30, y+5),QPointF(x+30+this->getName().count()*7, y+20)), this->getName());

    if(this->isPrimaryKey())
        painter->setBrush(Qt::black);
    painter->drawEllipse(x+20, y+9, 8, 8);
    painter->setBrush(Qt::white);

    if(this->isSecondaryKey()){
        painter->setBrush(Qt::black);
        painter->drawPie(x+20, y+9, 8, 8, 90*16, 180*16);
        painter->setBrush(Qt::white);
    }

    if(this->isNull())
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
    painter->drawLine(QPoint(x, y+12), QPoint(x+20, y+12));

    y+=15;
    foreach(Attribute *att, atts){
        painter->setPen(pen);
        y = att->addToScene(painter, x+28, y);
    }

    painter->setPen(pen);
    painter->drawLine(QPoint(x+28, dy+12), QPoint(x+28, y-3));

    return y;
}

void AttComp::addToXML(QDomDocument *document, QDomElement *root, QString parent)
{
    QDomElement element = document->createElement("Compound");
    element.setAttribute("name", this->getName());
    element.setAttribute("null", this->isNull());
    element.setAttribute("mult", this->isMultivalued());
    element.setAttribute("pk", this->isPrimaryKey());
    element.setAttribute("sk", this->isSecondaryKey());
    element.setAttribute("amount", this->amountAttributes);
    element.setAttribute("optionSQL", this->getOptionSQL());
    element.setAttribute("parent", parent);
    root->appendChild(element);
    foreach(Attribute *att, atts)
        att->addToXML(document, &element, this->getName());;
}

Attribute *AttComp::getCopy()
{
    Attribute *tmp = new AttComp(this->getName(), this->isNull(), this->amountAttributes, this->isMultivalued(), this->isPrimaryKey(), this->isSecondaryKey(), this->isChild());
    foreach(Attribute *att, atts)
        tmp->addAttribute(att->getCopy(), this->getName());
    return tmp;
}

void AttComp::addToTable(QTableWidget *table)
{
    QTableWidgetItem *a = new QTableWidgetItem(this->getName());
    QTableWidgetItem *e = new QTableWidgetItem(this->isMultivalued());
    QTableWidgetItem *c = new QTableWidgetItem();

    c->setFlags(Qt::NoItemFlags);
    if(this->isNull())
        c->setCheckState(Qt::Checked);
    else c->setCheckState(Qt::Unchecked);

    e->setFlags(Qt::NoItemFlags);
    if(this->isMultivalued())
        e->setCheckState(Qt::Checked);
    else e->setCheckState(Qt::Unchecked);

    AttDialog *dialog = (AttDialog*)table->parent();
    table->insertRow(table->rowCount());
    if(amountAttributes>atts.size()){
        table->setCellWidget(table->rowCount()-1, 0, new AddPushButton(QIcon(":/img/Plus.png"),QString(this->getName()), this, dialog));
        table->setItem(table->rowCount()-1, 1, a);
    }
    else {
        table->setItem(table->rowCount()-1, 0, a);
    }
    table->setItem(table->rowCount()-1, 1, c);
    table->setItem(table->rowCount()-1, 2, e);
}

void AttComp::addToSQL(QString &tmp, QString namePrnt, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints, Attribute *primaryKey, Attribute*)
{
    int a = this->getOptionSQL();
    if(a == 1){
        QString aux;
        foreach(Attribute *att, atts){
            att->addToSQL(tmp, namePrnt, tables, types, constraints, primaryKey, this);
            aux += att->getName()+", ";
        }
        aux.chop(2);
        if(this->isSecondaryKey())
            constraints.append("\n\tUNIQUE("+aux+"),");
    }
    else
    {
        QString aux;
        aux.append("CREATE TYPE t"+this->getName()+" AS (\n");
        foreach(Attribute *a, atts)
            a->addToSQL(aux, this->getName(), tables, types, constraints, primaryKey, this);
        aux.chop(2);
        aux.append("\n);");
        tmp.append("\t"+this->getName()+" t"+this->getName());
        if(!this->isNull())
            tmp.append(" NOT NULL");
        if(this->isSecondaryKey())
            tmp.append(" UNIQUE");
        tmp.append(",\n");
        if(types.indexOf(aux)==-1)
            types.append(aux);
    }
}

bool AttComp::moveAttribute(QString attribute, int row, int pos, QTableWidget *table)
{
    if (this->getName() == attribute && row >= 0){              //parent and all children must be moved in the UI
        if (pos < 0 || pos+atts.size() >= table->rowCount()) return false;

        if (pos < row)          //move upward
            for (int i = 0; i <= atts.size(); i++)
                moveRow(row+i,pos+i,table);
        else
            for (int i = atts.size(); i >= 0; i--)
                moveRow(row+i,pos+i,table);
        table->setCurrentCell(pos,0);
        return true;
    }

    int parentRow = 0;
    for (int i=0; i<table->rowCount(); i++)
        if (table->item(i,0)->text()== this->getName())
            parentRow = i;

    attribute = attribute.trimmed();
    foreach(Attribute * att, this->atts){                       //if it's a child attribute
        if (att->getName() == attribute && row >= 0
                && pos > parentRow && pos <= parentRow+atts.size()){
            moveRow(row,pos,table);
            int index = atts.indexOf(att);
            atts.removeOne(att);
            if (row < pos && index+1 <= atts.size())
                atts.insert(index+1,att);
            else
                if (index-1>=0)
                    atts.insert(index-1,att);
            return true;
        }
    }
    return false;
}

QList<QString> AttComp::getSQLName()
{
    QList<QString> aux;
    if(this->getOptionSQL() == 1){
        foreach(Attribute *a, atts)
            aux.append(a->getSQLName());
        return aux;
    }
    else aux.append(getName());
    return aux;
}

void AttComp::addToTableSQL(QTableWidget *table)
{
    QTableWidgetItem *a = new QTableWidgetItem(this->getName());
    table->insertRow(table->rowCount());
    table->setItem(table->rowCount()-1, 0, a);
    table->setCellWidget(table->rowCount()-1, 1, new AddEditButton(QIcon(":/img/Edit.png"),QString("Edit"), this, (QWidget*)table->parent()));
    foreach(Attribute *a, this->atts)
        a->addToTableSQL(table);

}
