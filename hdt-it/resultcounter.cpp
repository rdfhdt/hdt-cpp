#include "resultcounter.hpp"
#include <QtConcurrent/QtConcurrent>


ResultCounter::ResultCounter(QObject *parent, HDTController *controller) :
    QObject(parent),
    hdtController(controller),
    iterator(NULL)
{
}

void ResultCounter::startCounting()
{
    if(!hdtController->hasHDT()){
        return;
    }
    iterator = hdtController->getHDT()->getTriples()->search(hdtController->getSearchPatternID());
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
