#include "attsimple.h"

#include <QCheckBox>
#include <QPainter>

AttSimple::AttSimple(QString n, bool isNull, bool multivalued, bool primaryKey, bool secondaryKey, bool child):Attribute(n, isNull, multivalued, primaryKey, secondaryKey, child)
{
    type = "integer";
    cant1 = 20;
    cant2 = 0;
    cant3 = 2;
}

QList<QString> AttSimple::getAttsNames()
{
    QList<QString> a;
    a.push_back(this->getName());
    return a;
}

QString AttSimple::derive()
{
    QString tmp("");
    tmp.append(this->getName());
    if(this->isMultivalued()){
        tmp.push_front("{");
        tmp.append("}");
    }
    if(this->isNull())
        tmp.push_front(QChar(0x00AC));
    return tmp;
}

void AttSimple::getDeriveList(QList<Attribute *> arr[])
{
    if(this->isMultivalued())
        arr[Entity::multAtts].push_back(this);
    else arr[Entity::simpleAtts].push_back(this);
}

void AttSimple::addToTable(QTableWidget *table)
{
    QString aux = this->getName();
    if(this->isChild())
        aux = "      "+aux;

    QTableWidgetItem *a = new QTableWidgetItem(aux);
    QTableWidgetItem *b = new QTableWidgetItem();
    QTableWidgetItem *e = new QTableWidgetItem(this->isMultivalued());

    b->setFlags(Qt::NoItemFlags);
    if(this->isNull())
        b->setCheckState(Qt::Checked);
    else b->setCheckState(Qt::Unchecked);

    e->setFlags(Qt::NoItemFlags);
    if(this->isMultivalued())
        e->setCheckState(Qt::Checked);
    else e->setCheckState(Qt::Unchecked);

    table->insertRow(table->rowCount());
    table->setItem(table->rowCount()-1, 0, a);
    table->setItem(table->rowCount()-1, 1, b);
    table->setItem(table->rowCount()-1, 2, e);
}

bool AttSimple::addAttribute(Attribute*, QString)
{
    return false;
}

bool AttSimple::removeAttribute(QString)
{
    return false;
}

bool AttSimple::hasAttribute(QString attributesName)
{
    return (this->getName() == attributesName);
}

QList<Attribute *> AttSimple::getChildren()
{
    QList<Attribute *> a;
    return a;
}

bool AttSimple::canBePK()
{
    return !(this->isNull() || this->isMultivalued());
}

QList<Attribute *> AttSimple::getAttributes()
{
    QList<Attribute*> tmp;
    tmp.append(this);
    return tmp;
}

int AttSimple::addToScene(QPainter *painter, int x, int y)
{    
    QPen pen = painter->pen();
    painter->drawText(QRectF(QPoint(x+30, y+5),QPointF(x+30+this->getName().count()*7, y+20)), this->getName());

    if(this->isPrimaryKey())
        painter->setBrush(Qt::black);
    painter->drawEllipse(x+20, y+9, 8, 8);
    painter->setBrush(Qt::white);

    if(this->isSecondaryKey()){
        painter->setBrush(Qt::black);
        painter->drawPie(x+20, y+9, 8, 8, 90*16, 180*16);
        painter->setBrush(Qt::white);
    }

    if(this->isMultivalued()){
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawLine(QPoint(x+20, y+12), QPoint(x+15, y+7));
        painter->drawLine(QPoint(x+20, y+12), QPoint(x+15, y+17));
        painter->setPen(pen);
    }

    if(this->isNull())
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
    painter->drawLine(QPoint(x, y+12), QPoint(x+20, y+12));

    painter->setPen(pen);

    return y+15;
}

void AttSimple::addToXML(QDomDocument *document, QDomElement *root, QString parent)
{
    QDomElement element = document->createElement("Simple");
    element.setAttribute("name", this->getName());
    element.setAttribute("null", this->isNull());
    element.setAttribute("pk", this->isPrimaryKey());
    element.setAttribute("sk", this->isSecondaryKey());
    element.setAttribute("mult", this->isMultivalued());
    element.setAttribute("type", this->getType());
    element.setAttribute("cant1", cantA());
    element.setAttribute("cant2", cantB());
    element.setAttribute("cant3", cantC());
    element.setAttribute("optionSQL", this->getOptionSQL());
    element.setAttribute("parent", parent);
    root->appendChild(element);
}

Attribute *AttSimple::getCopy()
{
    Attribute *tmp = new AttSimple(this->getName(), this->isNull(), this->isMultivalued(), this->isPrimaryKey(), this->isSecondaryKey(), this->isChild());
    return tmp;
}

void AttSimple::addToTableSQL(QTableWidget *table)
{
    QString aux = this->getName();
    if(this->isChild())
        aux = "      "+aux;
    QTableWidgetItem *a = new QTableWidgetItem(aux);
    table->insertRow(table->rowCount());
    table->setItem(table->rowCount()-1, 0, a);
    table->setCellWidget(table->rowCount()-1, 1, new AddEditButton(QIcon(":/img/Edit.png"),QString("Edit"), this, (QWidget*)table->parent()));
}

bool AttSimple::moveAttribute(QString, int row, int pos, QTableWidget *table)
{
    return moveRow(row,pos,table);
}

void AttSimple::getSQLText(QString &text, QString number, Attribute *parent)
{
    QString type = this->getType();
    if(type == "varchar")
        type+="("+QString::number(this->cantA())+")";
    if(type == "numeric")
        type+="("+QString::number(this->cantA())+","+QString::number(this->cantB())+")";
    if(this->getOptionSQL() == 2)
        type.append("[]");
    text.append("\t"+this->getName()+number+" "+type);
    if(parent)
    qDebug() << this->getName() << parent->getOptionSQL();
    if(this->isPrimaryKey() || (this->getOptionSQL()==3) || (parent && parent->getOptionSQL()!=2 && !parent->isNull()) || (!parent && !this->isNull()))
        text.append(" NOT NULL");
    if(this->isSecondaryKey() && (!this->isMultivalued() || this->getOptionSQL() != 1))
        text.append(" UNIQUE");
    text.append(",\n");
}

void AttSimple::addToSQL(QString &tmp, QString namePrnt, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints, Attribute *primaryKey, Attribute *parent)
{
    if(this->isMultivalued()){
        int a = this->getOptionSQL();
        if(a == 1){ //add to main table
            QString aux;
            for(int i = 0 ; i<this->cantC() ; i++){
                this->getSQLText(tmp, "_"+QString::number(i), parent);
                aux += this->getName()+"_"+QString::number(i)+", ";
            }
            aux.chop(2);
            if(this->isSecondaryKey())
                constraints.append("\n\tUNIQUE("+aux+"),");
        }
        if(a == 2)
            this->getSQLText(tmp, "", parent);
        if(a == 3){ //new table
            QString aux;
            aux.append("CREATE TABLE "+this->getName()+primaryKey->getName()+"\n(\n");
            primaryKey->addToSQL(aux, this->getName()+primaryKey->getName(), tables, types, constraints, primaryKey, NULL);
            this->getSQLText(aux, "", parent);
            QString a;
            foreach(QString s, primaryKey->getAttsNames())
                a += s+", ";
            foreach(QString s, this->getAttsNames())
                a += s+", ";
            a.chop(2);
            QString name;
            foreach(QString s, primaryKey->getSQLName())
                name+=s+", ";
            name.chop(2);
            aux.append("\n\tCONSTRAINT PK_"+this->getName()+name+" PRIMARY KEY ("+name+", "+this->getName()+"),");
            aux.append("\n\tCONSTRAINT FK_"+this->getName()+name+" FOREIGN KEY ("+name+") REFERENCES "+namePrnt+"("+name+")");
            aux.append("\n);");
            tables.push_back(aux);
        }
    }
    else this->getSQLText(tmp, "", parent);
}

QList<QString> AttSimple::getSQLName()
{
    QList<QString> aux;
    aux.append(this->getName());
    return aux;
}

QString AttSimple::getType() const
{
    return this->type;
}

void AttSimple::setType(QString t, int a, int b, int c)
{
    this->type = t;
    this->cant1 = a;
    this->cant2 = b;
    this->cant3 = c;
}

int AttSimple::cantA() const
{
    return this->cant1;
}

int AttSimple::cantB() const
{
    return this->cant2;
}

int AttSimple::cantC() const
{
    return this->cant3;
}

