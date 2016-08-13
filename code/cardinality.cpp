#include "cardinality.h"

Cardinality::Cardinality(int min, QString max)
{
    this->min = min;
    this->max = max;
}

QString Cardinality::getText()
{
    return ("("+QString::number(min)+ "," + max+")");
}

int Cardinality::getMin()
{
    return this->min;
}

QString Cardinality::getMax()
{
    return this->max;
}

void Cardinality::setMin(int min)
{
    this->min = min;
}

void Cardinality::setMax(QString max)
{
    this->max = max;
}

/**
 * Return TRUE if max is Multiple.
 * Meaning different from 1.
 */
bool Cardinality::maxIsMultiple(){
    return (max != "1");
}

