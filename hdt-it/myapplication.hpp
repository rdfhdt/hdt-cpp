#ifndef MYAPPLICATION_HPP
#define MYAPPLICATION_HPP

#include <QApplication>
#include "hdtit.hpp"

class MyApplication : public QApplication
{
    Q_OBJECT
private:
    HDTit *hdtIt;
    void loadFile(QString fileName);
public:
    explicit MyApplication(int &argc, char **argv);
    bool event(QEvent *);
    void setHDTit(HDTit *hdtIt);
    bool notify(QObject *, QEvent *);
signals:

public slots:

};

#endif // MYAPPLICATION_HPP
