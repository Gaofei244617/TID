#include "tid.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TID w;
    w.show();
    return a.exec();
}
