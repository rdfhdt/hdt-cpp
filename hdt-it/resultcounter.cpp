#include "resultcounter.hpp"
#include <QtConcurrentRun>


ResultCounter::ResultCounter(QObject *parent, HDTManager *manager) :
    QObject(parent),
    manager(manager),
    iterator(NULL)
{
}

void ResultCounter::startCounting()
{
    if(!manager->hasHDT()){
        return;
    }
    iterator = manager->getHDT()->getTriples().search(manager->getSearchPatternID());
    shouldCancel = false;
    QtConcurrent::run(this, &ResultCounter::run);
}

void ResultCounter::cancel()
{
    shouldCancel = true;
}

void ResultCounter::run()
{
    if(iterator==NULL) {
        return;
    }
    StopWatch cl;
    unsigned int numResults = 0;
    while(!shouldCancel && iterator->hasNext()) {
        iterator->next();
        numResults++;

        cl.stop();
        if(cl.getReal()>10000) {
            cl.reset();
            emit numResultsChanged(numResults);
        }
    }
    delete iterator;
    iterator = NULL;
    emit finished();
}
