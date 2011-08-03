#ifndef PREDICATESTATUS_HPP
#define PREDICATESTATUS_HPP

#include <QObject>
#include "hdtmanager.hpp"

class PredicateStatus : public QObject
{
    Q_OBJECT
private:
    vector<bool> activePredicate;
    int minPredicateCount;
    int maxPredicateCount;
    HDTManager *manager;

    void setMinimumPredicateCountInternal(int count);
public:
    explicit PredicateStatus(QObject *parent, HDTManager *manager);

    unsigned int getMinimumPredicateCount();
    unsigned int getMaximumPredicateCount();

    bool isPredicateActive(int i);
    void setPredicateActive(int i, bool b);

signals:
    void predicatesChanged(unsigned int min, unsigned int max);
    void minimumPredicateCountChanged(int newval);

public slots:
    void refreshAll();
    void selectPredicate(unsigned int pred);
    void selectAllPredicates();
    void selectNonePredicates();
    void setMinimumPredicateCount(int count);
};

#endif // PREDICATESTATUS_HPP
