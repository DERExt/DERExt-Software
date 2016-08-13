#include "sqldialog.h"
#include "ui_sqldialog.h"

SQLDialog::SQLDialog(QList<ERItem *> list, Error *error, QWidget *parent) :QDialog(parent),ui(new Ui::SQLDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("SQL Derivation");
    this->items = list;
    this->error = error;;

    this->initializeTables();

    for(int i=0 ; i<items.size() ; i++){
        this->ui->comboBox->addItem(items[i]->getName());
        if (items[i]->getFKs().size()>1)    //only entities can posibly return more than 1 rship in FKs
            avoidRepetition(items[i]->getFKs(),(Entity*)items[i]);

        //If it's unary, must ask for a rolename.
        if (items[i]->getDerivation().left(1)!="D"&&items[i]->getDerivation().left(1)!="R"){ //not an entity
            if (((Relationship*)items[i])->getEntities().at(0)->getName()==((Relationship*)items[i])->getEntities().at(1)->getName()
                    && ((Relationship*)items[i])->getRolename().isEmpty()){
                bool ok;
                QString roleName = QInputDialog::getText(this, tr("Rolename"),
                                                         "Insert the rolename needed to derive the unary relationship '" + items[i]->getName() +"'."
                                                         , QLineEdit::Normal,
                                                         "rolename", &ok);
                if (ok && !roleName.isEmpty())
                    ((Relationship*)items[i])->setRolename(roleName);
                else
                    if (ok) ((Relationship*)items[i])->setRolename(items[i]->getName()+"RN");
            }
        }
    }
}

SQLDialog::~SQLDialog()
{
    delete ui;
}

void SQLDialog::on_buttonBox_accepted()
{
    QString aux;
    foreach(ERItem *e, items){
        aux += e->getSQL() + "\n";
    }
    QString filter = "DERExt (*.sql)";
    QString path = QFileDialog::getSaveFileName(this, "Save files", QDir::currentPath()+".sql", "*.sql", &filter);
    if (path.right(4)!=".sql") path+=".sql";
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        stream << aux;
        file.close();
    }
}

void SQLDialog::on_buttonBox_rejected()
{
    this->close();
}

void SQLDialog::changeItem(QString name)
{
    QList<Attribute *> aux;
    ERItem * item;
    bool found = false;
    for(int i=0 ; i<items.size() && !found ; i++)
        if(items[i]->getName() == name){
            found = true;
            aux = items[i]->getAttributes();
            item = items[i];
        }
    foreach(Attribute *a, aux){
        if(a->getChildren().isEmpty()){
            if(a->isMultivalued())
                a->addToTableSQL(ui->MultTable);
            else a->addToTableSQL(ui->SimpleTable);
        }
        else a->addToTableSQL(ui->CompTable);
    }
    foreach(Relationship * r,item->getFKs())
        r->addToTableSQL(ui->fkTable);
}

void SQLDialog::on_comboBox_currentIndexChanged(const QString &name)
{

    this->ui->SimpleTable->clear();
    this->ui->CompTable->clear();
    this->ui->MultTable->clear();
    this->initializeTables();
    this->changeItem(name);
}

void SQLDialog::initializeTables()
{
    QStringList tableHeader;
    tableHeader<<"Name"<<"Edition";
    ui->SimpleTable->setRowCount(0);
    ui->SimpleTable->setColumnCount(2);
    ui->SimpleTable->setHorizontalHeaderLabels(tableHeader);
    ui->SimpleTable->verticalHeader()->setVisible(false);
    ui->SimpleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SimpleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SimpleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->SimpleTable->setShowGrid(true);
    ui->SimpleTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);
    ui->SimpleTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ui->CompTable->setRowCount(0);
    ui->CompTable->setColumnCount(2);
    ui->CompTable->setHorizontalHeaderLabels(tableHeader);
    ui->CompTable->verticalHeader()->setVisible(false);
    ui->CompTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->CompTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->CompTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->CompTable->setShowGrid(true);
    ui->CompTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);
    ui->CompTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ui->MultTable->setRowCount(0);
    ui->MultTable->setColumnCount(2);
    ui->MultTable->setHorizontalHeaderLabels(tableHeader);
    ui->MultTable->verticalHeader()->setVisible(false);
    ui->MultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->MultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->MultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->MultTable->setShowGrid(true);
    ui->MultTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);
    ui->MultTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ui->fkTable->setRowCount(0);
    ui->fkTable->setColumnCount(4);
    tableHeader.clear();
    tableHeader << "Name" << "On delete" << "On update" << "Matching";
    ui->fkTable->setHorizontalHeaderLabels(tableHeader);
    ui->fkTable->verticalHeader()->setVisible(false);
    ui->fkTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fkTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fkTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->fkTable->setShowGrid(true);
    ui->fkTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);
    ui->fkTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

/**
 * @brief SQLDialog::avoidRepetition
 * In order to avoid repetition in the FKs names in the SQL deriv of an Entity,
 * ask for rolenames to replace such.
 * Done for relationships N:1
 */
void SQLDialog::avoidRepetition(QList<Relationship *> rships, Entity *ent)
{
    QList<Entity*> ents;
    foreach(Relationship * r, rships){      //get the other entity in the relationship
        if (r->getEntities().size()==2){
            if (r->getEntities().at(0)->getName()==ent->getName())
                ents << r->getEntities().at(1);
            else ents << r->getEntities().at(0);
        }
    }

    bool found = false;
    for (int i = 0; i < ents.size(); i++){       //for each entity
        int j = i+1;
        while (j < ents.size() && !found){          //see if there is repetition in the rest of the list
            if (ents.at(j)->getName()==ents.at(i)->getName()
                    && rships.at(j)->getRolename().isEmpty()){
                bool ok;
                QString roleName = QInputDialog::getText(this, tr("Rolename"),
                                                         "There are multiple N:1 relationships between entities "
                                                            +ent->getName()+" and "+ents.at(j)->getName()
                                                            +". A rolename is needed for the foreign key of relationship "
                                                            +rships.at(j)->getName()+"."
                                                         ,QLineEdit::Normal, "rolename", &ok);
                if (ok && !roleName.isEmpty())
                    rships.at(j)->setRolename(roleName);
                found = true;
            }
            j++;
        }
        found = false;
    }
}

void SQLDialog::on_savePButton_clicked()
{
    QString filter = "DERExt (*.eleT)";
    QString path = QFileDialog::getSaveFileName(this, "Save files", QDir::currentPath()+".eleT", "*.eleT", &filter);
    if (path.right(5)!=".eleT") path+=".eleT";
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        QDomDocument document;
        QDomElement root;
        root = document.createElement("DERExt");
        foreach(ERItem* ent, items)
            root.appendChild(*ent->getXML(&document));
        document.appendChild(root);
        stream << document.toString();
        file.close();
    }
}
