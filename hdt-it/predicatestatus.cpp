#include "predicatestatus.hpp"

PredicateStatus::PredicateStatus(HDTController *manager) :
    controller(manager), minPredicateCount(0), maxPredicateCount(0)
{
}

unsigned int PredicateStatus::getMinimumPredicateCount()
{
    return minPredicateCount;
}

unsigned int PredicateStatus::getMaximumPredicateCount()
{
    return maxPredicateCount;
}

bool PredicateStatus::isPredicateActive(int i)
{
    return activePredicate[i];
}

void PredicateStatus::setPredicateActive(int i, bool b)
{
    if(activePredicate[i]!=b) {
        activePredicate[i] = b;
        setMinimumPredicateCountInternal(0);
        emit predicatesChanged(i,i);
    }
}

void PredicateStatus::refreshAll()
{
    activePredicate.clear();
    if(controller->hasHDT()) {
        activePredicate.resize(controller->getHDT()->getDictionary()->getNpredicates(), true);
	this->maxPredicateCount = controller->getHDTCachedInfo()->getMaxPredicateCount();
        setMinimumPredicateCountInternal(0);
    }
}

void PredicateStatus::selectAllPredicates()
{
//    cout << "selectAllPredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = true;
    }
    emit predicatesChanged(0, activePredicate.size());
    setMinimumPredicateCountInternal(0);
}

void PredicateStatus::selectNonePredicates()
{
//    cout << "selectNonePredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = false;
    }
    emit predicatesChanged(0, activePredicate.size());
    setMinimumPredicateCountInternal(0);
}

void PredicateStatus::selectPredicate(unsigned int pred)
{
    if(pred==0) {
        selectAllPredicates();
        return;
    }

    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = i==(pred-1);
    }
    emit predicatesChanged(0, activePredicate.size());
    setMinimumPredicateCountInternal(0);
    emit predicateSelected(pred-1);
}

// Private, just updates the value.
void PredicateStatus::setMinimumPredicateCountInternal(int count)
{
    if(count!=minPredicateCount) {
        minPredicateCount = count;
        emit minimumPredicateCountChanged(count);
    }
}

// Public, update all activePredicate, values.
void PredicateStatus::setMinimumPredicateCount(int count)
{
    if(count!=minPredicateCount) {
        minPredicateCount = count;
        for(unsigned int i=0;i<activePredicate.size();i++) {
	    activePredicate[i] = controller->getHDTCachedInfo()->getPredicateUsages(i)>=(unsigned int)count;
        }
        emit predicatesChanged(0, activePredicate.size());
        emit minimumPredicateCountChanged(count);
    }
}
