
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include "myapplication.hpp"
#include "hdtit.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
#ifdef __APPLE__
    myappTranslator.load("../Resources/hdt-it_"+QLocale::system().name());
#else
    myappTranslator.load("hdt-it_"+QLocale::system().name());
#endif
    a.installTranslator(&myappTranslator);

    HDTit w;

    a.setHDTit(&w);

    w.show();

    if(argc>1) {
	QString str(argv[1]);
	w.openHDTFile(str);
    }

    return a.exec();
}
