#include "valuedialog.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

ValueDialog::ValueDialog(int mode) {
    QString data;
    if(mode == 0) {
        data = "income";
    } else {
        data = "expense";
    }
    setWindowTitle("Enter " + data);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Enter " + data);
    layout->addWidget(label);
    input = new QLineEdit();
    layout->addWidget(input);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
