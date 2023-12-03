#include "EdgeDetection.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EdgeDetection w;
    w.show();
    return a.exec();
}
