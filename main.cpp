#include <QApplication>
#include "appwidget.h"

int main(int argc, char** argv) {
    QApplication app(argc,argv);
    AppWidget widget;
    widget.show();
    return app.exec();
}
