#include "hdtcachedinfo.hpp"
#include "constants.h"

HDTCachedInfo::HDTCachedInfo(hdt::HDT *hdt) : hdt(hdt)
{

}

void HDTCachedInfo::loadInfo(hdt::ProgressListener *listener)
{
    unsigned int nPred = hdt->getDictionary().getNpredicates();
    maxPredicateCount = 0;
    numResults = 0;
    predicateCount.clear();
    predicateCount.resize(nPred);
    hdt::Triples &t = hdt->getTriples();

    unsigned int increment = t.getNumberOfElements()/RENDER_NUM_POINTS;
    increment = increment < 1 ? 1 : increment;

    resultsTime.reset();
    hdt::IteratorTripleID *it = t.searchAll();

    while(it->hasNext()) {
        hdt::TripleID *tid = it->next();

        if( (numResults%increment)==0) {
            triples.push_back(*tid);
        }

        //subjectCount[tid->getSubject()-1]++;
        predicateCount[tid->getPredicate()-1]++;
        //objectCount[tid->getObject()-1]++;

        if(maxPredicateCount<predicateCount[tid->getPredicate()-1]) {
            maxPredicateCount = predicateCount[tid->getPredicate()-1];
        }
        numResults++;

        NOTIFYCOND(listener, "PostProcessing HDT data", numResults, t.getNumberOfElements());
    }
    resultsTime.stop();
    delete it;

    // Calculate Predicate Colors
    StopWatch s;
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
