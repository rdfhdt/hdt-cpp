#include "hdtcachedinfo.hpp"
#include "constants.h"

#include <fstream>

HDTCachedInfo::HDTCachedInfo(hdt::HDT *hdt) : hdt(hdt)
{

}

void HDTCachedInfo::generateInfo(hdt::ProgressListener *listener)
{

    hdt::Triples *t = hdt->getTriples();

    // Count predicates
    unsigned int nPred = hdt->getDictionary()->getNpredicates();

    hdt::TripleID triplePredicate;
    maxPredicateCount = 0;
    predicateCount.clear();
    predicateCount.resize(nPred);

    for(int p=1;p<=nPred;p++) {
	triplePredicate.setAll(0, p, 0);
    hdt::IteratorTripleID *predIt = t->search(triplePredicate);

	predicateCount[p] = predIt->estimatedNumResults();

	maxPredicateCount = max(maxPredicateCount, predicateCount[p]);

	cout << "Predicate " << p << " Count: " << predicateCount[p] << endl;

	delete predIt;
    }

    // Generate array
    unsigned int increment = t->getNumberOfElements()/RENDER_NUM_POINTS;
    increment = increment < 1 ? 1 : increment;

    hdt::IteratorTripleID *it = t->searchAll();

    for(int i=0;i<t->getNumberOfElements();i+=increment) {
	it->goTo(i);
	hdt::TripleID *tid = it->next();
	triples.push_back(*tid);

    NOTIFYCOND(listener, "Generating Matrix", i, t->getNumberOfElements());
    }

    delete it;

    // Calculate Predicate Colors
    predicateColors.clear();
    predicateColors.resize(nPred);
    ColorRamp2 cr;
    for(unsigned int i=0;i<nPred; i++) {
	cr.apply(&predicateColors[i], i, 0, nPred-1);
    }
}

Color * HDTCachedInfo::getPredicateColor(unsigned int npred)
{
    return &predicateColors[npred];
}

unsigned int HDTCachedInfo::getPredicateUsages(unsigned int predicate)
{
    return predicateCount[predicate];
}

unsigned int HDTCachedInfo::getMaxPredicateCount()
{
    return maxPredicateCount;
}

vector<hdt::TripleID> &HDTCachedInfo::getTriples()
{
    return triples;
}
