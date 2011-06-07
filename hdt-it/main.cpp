#include <QtGui/QApplication>
#include "hdtit.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HDTit w;
    w.show();

    return a.exec();
}
