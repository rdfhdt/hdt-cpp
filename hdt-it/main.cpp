#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include "hdtit.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    bool ok1 = qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    bool ok2 = myappTranslator.load("hdt-it_"+QLocale::system().name());
    a.installTranslator(&myappTranslator);

    std::cout << "Sys: " << ok1 << " App: " << ok2 << endl;

    std::cout << "Locale: " << (char *)QLocale::system().name().toAscii().data() << std::endl;

    HDTit w;
    w.show();

    return a.exec();
}
