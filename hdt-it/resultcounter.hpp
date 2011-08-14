#ifndef RESULTCOUNTER_HPP
#define RESULTCOUNTER_HPP

#include <QObject>

#include <hdtmanager.hpp>

class ResultCounter : public QObject
{
    Q_OBJECT
private:
    HDTManager *manager;
    hdt::IteratorTripleID *iterator;
    bool shouldCancel;
public:
    explicit ResultCounter(QObject *parent, HDTManager *manager);

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
