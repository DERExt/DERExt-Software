#ifndef CARDINALITY_H
#define CARDINALITY_H

#include <QtCore>

class Cardinality
{
private:
    int min;
    QString max;        //this allows for numbers bigger than 9 and characters (e.g. N)

public:
    Cardinality(int min, QString max);

    QString getText();
    int getMin();
    QString getMax();

    void setMin(int min);
    void setMax(QString max);
    bool maxIsMultiple();
};

#endif // CARDINALITY_H
