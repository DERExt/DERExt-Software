#ifndef ATTCOMP
#define ATTCOMP

#include "attribute.h"
#include "entity.h"
#include "addeditbutton.h"

#include <QList>

class AttComp : public Attribute
{
private:
    QList<Attribute*> atts;
    int amountAttributes;

public:
    AttComp(QString attributeName, bool isNull, int amountAttributes, bool multivalued, bool primaryKey, bool secondaryKey, bool child);
    QList<QString> getAttsNames();
    QString derive();
    void getDeriveList(QList<Attribute*> arr[]);
    void addToTable(QTableWidget *table);
    bool addAttribute(Attribute *attribute, QString parent);
    bool removeAttribute(QString attributesName);
    bool hasAttribute(QString attributesName);
    QList<Attribute *> getChildren();
    bool canBePK();
    QList<Attribute*> getAttributes();
    QList<Attribute*> getAllAttributes();
    int addToScene(QPainter *painter, int x, int y);
    void addToXML(QDomDocument *document, QDomElement *root, QString parent);
    Attribute *getCopy();
    void addToTableSQL(QTableWidget *table);
    QList<QString> getSQLName();
    void addToSQL(QString &tmp, QString namePrnt, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints, Attribute *primaryKey, Attribute *parent);
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget *table);
};

#endif // ATTCOMP

