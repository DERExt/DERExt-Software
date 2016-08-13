#ifndef ATTSIMPLE
#define ATTSIMPLE

#include "attribute.h"
#include "entity.h"
#include "addeditbutton.h"

#include <QList>
#include <QtXml/QDomDocument>

class AttSimple : public Attribute
{
public:
    AttSimple(QString attributeName, bool isNull, bool multivalued, bool primaryKey, bool secondaryKey, bool child);
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
    int addToScene(QPainter *painter, int x, int y);
    void addToXML(QDomDocument *document, QDomElement *root, QString parent);
    Attribute *getCopy();

    int cantC() const;
    int cantB() const;
    int cantA() const;
    void setType(QString t, int a, int b, int c);
    QString getType() const;
    QList<QString> getSQLName();
    void addToSQL(QString &tmp, QString namePrnt, QList<QString> &tables, QList<QString> &types, QList<QString> &constraints, Attribute *primaryKey, Attribute *parent);
    void getSQLText(QString &text, QString number, Attribute *parent);
    void addToTableSQL(QTableWidget *table);
    bool moveAttribute(QString attribute, int row, int pos, QTableWidget *table);
private:
    void getSQLText(QString *text);
    QString type;
    int cant1, cant2, cant3;
};

#endif // ATTSIMPLE

