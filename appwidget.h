#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>

class AppWidget : public QWidget
{
    Q_OBJECT
public:
    AppWidget();
    QTableWidget* incomeTable;
    QTableWidget* expenseTable;
    QLabel* totalLabel;
private slots:
    void NewIncome();
    void NewExpense();
    void UpdateTotal();
    void removeFromTable(QTableWidget* table, bool isExpense);
    void averageFromTable(QTableWidget* table);
    void saveJson();
    void loadJson();
};

#endif // APPWIDGET_H
