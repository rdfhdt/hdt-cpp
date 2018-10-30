#ifndef PREDICATESTATUS_HPP
#define PREDICATESTATUS_HPP

#include <QObject>
#include "hdtcontroller.hpp"

class PredicateStatus : public QObject
{
    Q_OBJECT
private:
    vector<bool> activePredicate;
    size_t minPredicateCount;
    size_t maxPredicateCount;
    HDTController *controller;

    void setMinimumPredicateCountInternal(int count);
public:
    explicit PredicateStatus(HDTController *controller);

    size_t getMinimumPredicateCount();
    size_t getMaximumPredicateCount();

    bool isPredicateActive(size_t i);
    void setPredicateActive(size_t i, bool b);

signals:
    void predicatesChanged(size_t min, size_t max);
    void minimumPredicateCountChanged(size_t newval);
    void predicateSelected(size_t npred);

public slots:
    void refreshAll();
    void selectPredicate(size_t pred);
    void selectAllPredicates();
    void selectNonePredicates();
    void setMinimumPredicateCount(int count);
};

#endif // PREDICATESTATUS_HPP
