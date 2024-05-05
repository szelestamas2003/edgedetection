#include "EdgeDetection.h"
#include <QtWidgets/QApplication>
#include "qstylefactory.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("windowsvista"));
    EdgeDetection w;
    w.show();
    return a.exec();
}
