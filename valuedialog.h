#include <QDialog>
#include <QLineEdit>

#ifndef VALUEDIALOG_H
#define VALUEDIALOG_H

class ValueDialog : public QDialog
{
public:
    QLineEdit* input;
    ValueDialog(int mode);
};

#endif // VALUEDIALOG_H
