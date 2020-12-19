#include "tid.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TID w;
    w.show();
    w.resize(900, 600);
    w.setHint("Drag File Here");
    return a.exec();
}
