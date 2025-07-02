#include "appwidget.h"
#include "valuedialog.h"
#include "globalvalues.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMessageBox>
#include <QDateTime>
#include <QTableWidget>
#include <QGroupBox>
#include <QItemSelectionModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>

void AppWidget::removeFromTable(QTableWidget* table, bool isExpense) {
    QItemSelectionModel *selections = table->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    if(selected.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("No rows selected!");
        msgBox.exec();
        return;
    }
    QList<int> rowsToRemove;
    for(int i = 0; i < selected.size(); i++) {
        rowsToRemove.append(selected[i].row());
    }
    std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());
    for(int row : rowsToRemove) {
        int value = table->item(row,0)->text().toInt();
        table->removeRow(row);
        if(isExpense) {
            expenseTotal -= value;
        } else {
            incomeTotal -= value;
        }
    }
    UpdateTotal();
    QMessageBox msgBox;
    msgBox.setText("Removed " + QString::number(selected.size()) + " rows!");
    msgBox.exec();
}

void AppWidget::averageFromTable(QTableWidget* table) {
    QItemSelectionModel *selections = table->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    if(selected.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("No rows selected!");
        msgBox.exec();
        return;
    }
    float average = 0.0F;
    for(int i = 0; i < selected.size(); i++) {
        int row = selected[i].row();
        float value = table->item(row,0)->text().toFloat();
        average += value;
    }
    average /= selected.size(); // average is sum of all divided by length
    QMessageBox msgBox;
    msgBox.setText("Average: $" + QString::number(average));
    msgBox.exec();
}

AppWidget::AppWidget() {
    QVBoxLayout* layoutLeftPanel = new QVBoxLayout();
    QHBoxLayout* layoutTables = new QHBoxLayout();;
    QHBoxLayout* layout = new QHBoxLayout(this);

    //init controls
    QPushButton* btnRegIncome = new QPushButton("Add income");
    QPushButton* btnRegExpense = new QPushButton("Add expense");
    QPushButton* saveFile = new QPushButton("Save this session");
    totalLabel = new QLabel();
    QGroupBox* incomeBox = new QGroupBox("Incomes");
    QGroupBox* expenseBox = new QGroupBox("Expenses");
    incomeTable = new QTableWidget();
    expenseTable = new QTableWidget();
    QPushButton* removeSelectedIncome = new QPushButton("Remove selected income values");
    QPushButton* removeSelectedExpense = new QPushButton("Remove selected expense values");
    QPushButton* calculateAverageIncome = new QPushButton("Calculate average income based on selected");
    QPushButton* calculateAverageExpense = new QPushButton("Calculate average expense based on selected");

    incomeTable->setHorizontalHeaderLabels({"Date/Time","Value"});
    expenseTable->setHorizontalHeaderLabels({"Date/Time","Value"});
    incomeTable->setRowCount(0);
    incomeTable->setColumnCount(2);
    expenseTable->setRowCount(0);
    expenseTable->setColumnCount(2);

    QVBoxLayout* layoutIncome = new QVBoxLayout();
    layoutIncome->addWidget(incomeTable);
    layoutIncome->addWidget(calculateAverageIncome);
    layoutIncome->addWidget(removeSelectedIncome);
    incomeBox->setLayout(layoutIncome);

    QVBoxLayout* layoutExpense = new QVBoxLayout();
    layoutExpense->addWidget(expenseTable);
    layoutExpense->addWidget(calculateAverageExpense);
    layoutExpense->addWidget(removeSelectedExpense);
    expenseBox->setLayout(layoutExpense);

    btnRegIncome->setMaximumSize(200,100);
    btnRegIncome->setMaximumHeight(100);
    btnRegExpense->setMaximumSize(200,100);
    btnRegExpense->setMaximumHeight(100);

    btnRegIncome->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    btnRegExpense->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

    connect(btnRegIncome, &QPushButton::released,this,&AppWidget::NewIncome);
    connect(btnRegExpense, &QPushButton::released,this,&AppWidget::NewExpense);
    connect(removeSelectedIncome, &QPushButton::released,this,[=]() {
        removeFromTable(incomeTable, false);
    });
    connect(removeSelectedExpense, &QPushButton::released,this,[=](){
        removeFromTable(expenseTable, true);
    });
    connect(calculateAverageIncome, &QPushButton::released,this,[=]() {
        averageFromTable(incomeTable);
    });
    connect(calculateAverageExpense, &QPushButton::released,this,[=]() {
        averageFromTable(expenseTable);
    });
    connect(saveFile,&QPushButton::released,this,[=]() {
        saveJson();
    });

    layoutLeftPanel->addWidget(btnRegIncome, 1);
    layoutLeftPanel->addWidget(btnRegExpense, 1);
    layoutLeftPanel->addWidget(saveFile,1);
    const QFont font("Arial",25);
    totalLabel->setFont(font);
    layoutLeftPanel->addWidget(totalLabel,1);
    layoutLeftPanel->addStretch(1);

    layoutTables->addWidget(incomeBox);
    layoutTables->addWidget(expenseBox);

    layout->addLayout(layoutLeftPanel);
    layout->addLayout(layoutTables);

    setLayout(layout);
    loadJson(); // load data
    UpdateTotal(); // init total label
}

void valueIsNotAllowed() {
    QMessageBox msgBox;
    msgBox.setText("Value is not allowed!");
    msgBox.exec();
}

void addValue(QTableWidget* table, int value, QDateTime dateTime) {
    int rowCount = table->rowCount();
    table->insertRow(rowCount);
    table->setItem(rowCount,0,new QTableWidgetItem(QString::number(value)));
    table->setItem(rowCount,1,new QTableWidgetItem(dateTime.toString()));
}

void AppWidget::saveJson() {
    QJsonArray incomeArray;
    QJsonArray expenseArray;
    for(int i = 0; i < incomeTable->rowCount(); i++) {
        QJsonObject entry;
        entry["value"] = incomeTable->item(i,0)->text();
        entry["datetime"] = incomeTable->item(i,1)->text();
        incomeArray.append(entry);
    }
    for(int i = 0; i < expenseTable->rowCount(); i++) {
        QJsonObject entry;
        entry["value"] = expenseTable->item(i,0)->text();
        entry["datetime"] = expenseTable->item(i,1)->text();
        expenseArray.append(entry);
    }

    QJsonObject root;
    root["income"] = incomeArray;
    root["expense"] = expenseArray;

    QJsonDocument doc(root);

    QDir dir = QDir::current();
    QString fileName = "data.json";
    QString fullFilePath = dir.filePath(fileName);

    QFile file(fullFilePath);
    if(file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Error saving file!");
        msgBox.exec();
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("File saved successfully");
    msgBox.exec();
}

void AppWidget::loadJson() {
    QDir dir = QDir::current();
    QString fileName = "data.json";
    QString fullFilePath = dir.filePath(fileName);

    QFile file(fullFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox msgBox;
        msgBox.setText("Error reading from file! Cannot load data!");
        msgBox.exec();
        return;
    }

    QByteArray rawData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    file.close();

    if(!doc.isObject()) {
        QMessageBox msgBox;
        msgBox.setText("Invalid file!");
        msgBox.exec();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray incomeArray = root["income"].toArray();
    QJsonArray expenseArray = root["expense"].toArray();

    incomeTable->setRowCount(0); // clear table

    for (int i = 0; i < incomeArray.size(); ++i) {
        QJsonObject entry = incomeArray[i].toObject();
        QString datetime = entry["datetime"].toString();
        QString value = entry["value"].toString();

        int row = incomeTable->rowCount();
        incomeTable->insertRow(row);
        incomeTable->setItem(row, 1, new QTableWidgetItem(datetime));
        incomeTable->setItem(row, 0, new QTableWidgetItem(value));

        incomeTotal += value.toFloat();
    }

    for (int i = 0; i < expenseArray.size(); ++i) {
        QJsonObject entry = expenseArray[i].toObject();
        QString datetime = entry["datetime"].toString();
        QString value = entry["value"].toString();

        int row = expenseTable->rowCount();
        expenseTable->insertRow(row);
        expenseTable->setItem(row, 1, new QTableWidgetItem(datetime));
        expenseTable->setItem(row, 0, new QTableWidgetItem(value));

        expenseTotal += value.toFloat();
        QDateTime dateTime = QDateTime::currentDateTime();
        //expenseData.append(qMakePair(value,dateTime));
    }
}

void AppWidget::NewIncome() {
    ValueDialog dlg(0);
    if(dlg.exec() == QDialog::Accepted) {
        QString rawValue = dlg.input->text();
        bool ok;
        int value = rawValue.toInt(&ok);
        if(!ok) {
            valueIsNotAllowed();
            return;
        }
        if(value <= 0) {
            valueIsNotAllowed();
            return;
        }
        QDateTime dateTime = QDateTime::currentDateTime();
        incomeData.append(qMakePair(value,dateTime));
        addValue(incomeTable,value,dateTime);
        incomeTotal += value;
        UpdateTotal();
    }
}
void AppWidget::NewExpense() {
    ValueDialog dlg(1);
    if(dlg.exec() == QDialog::Accepted) {
        QString rawValue = dlg.input->text();
        bool ok;
        int value = rawValue.toInt(&ok);
        if(!ok) {
            valueIsNotAllowed();
            return;
        }
        if(value <= 0) {
            valueIsNotAllowed();
            return;
        }
        QDateTime dateTime = QDateTime::currentDateTime();
        expenseData.append(qMakePair(value,dateTime));
        addValue(expenseTable,value,dateTime);
        expenseTotal += value;
        UpdateTotal();
    }
}
void AppWidget::UpdateTotal() {
    int newTotal = incomeTotal - expenseTotal; // income - expense = total $
    totalLabel->setText("$"+QString::number(newTotal));
    if(newTotal < 0) {
        QMessageBox msgBox;
        msgBox.setText("More expenses than income! Negative total."); // why not?
        msgBox.exec();
    }
}
