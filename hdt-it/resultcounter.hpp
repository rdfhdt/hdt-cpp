#ifndef RESULTCOUNTER_HPP
#define RESULTCOUNTER_HPP

#include <QObject>

#include <hdtcontroller.hpp>

class ResultCounter : public QObject
{
    Q_OBJECT
private:
    HDTController *hdtController;
    hdt::IteratorTripleID *iterator;
    bool shouldCancel;
public:
    explicit ResultCounter(QObject *parent, HDTController *hdtController);

    void run();
signals:
    void numResultsChanged(int numResults);
    void messageChanged(QString message);
    void finished();
private slots:
    void startCounting();
    void cancel();
public slots:

};

#endif // RESULTCOUNTER_HPP
