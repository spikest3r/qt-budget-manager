#include <QList>
#include <QDateTime>
#include <QPair>

#ifndef GLOBALVALUES_H
#define GLOBALVALUES_H

extern QList<QPair<int,QDateTime>> incomeData;
extern QList<QPair<int,QDateTime>> expenseData;
extern int incomeTotal;
extern int expenseTotal;

#endif // GLOBALVALUES_H
