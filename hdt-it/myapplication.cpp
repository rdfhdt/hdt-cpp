#include "myapplication.hpp"

MyApplication::MyApplication(int &argc, char **argv) : QApplication(argc, argv), hdtIt(NULL)
{
}

bool MyApplication::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        loadFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    default:
        return QApplication::event(event);
    }
}

void MyApplication::loadFile(QString fileName)
{
    if(hdtIt) {
        hdtIt->openHDTFile(fileName);
    }
}

void MyApplication::setHDTit(HDTit *hdtIt)
{
    this->hdtIt=hdtIt;
}

bool MyApplication::notify(QObject *o, QEvent *e)
{
    try {
        return QApplication::notify(o, e);
    } catch (char *err) {
        std::cerr << "Exception caugth in notify (char *): " << err << endl;
    } catch (const char *err) {
        std::cerr << "Exception caugth in notify (const char *): " << err << endl;
    } catch (const std::exception& ex) {
        std::cerr << "Exception caugth in notify (std::exception &): " << ex.what() << endl;
    } catch (const std::string& ex) {
        std::cerr << "Exception caugth in notify (std::string &): " << ex << endl;
    } catch (...) {
        std::cerr << "Unknown Exception caugth in notify: " << endl;
    }
}
