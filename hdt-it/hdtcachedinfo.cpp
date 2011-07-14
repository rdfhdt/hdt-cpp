#include "hdtcachedinfo.hpp"

HDTCachedInfo::HDTCachedInfo(hdt::HDT *hdt) : hdt(hdt)
{

}

void HDTCachedInfo::loadInfo(hdt::ProgressListener *listener)
{
    maxPredicateCount = 0;
    numResults = 0;
    predicateCount.clear();
    predicateCount.resize(hdt->getDictionary().getNpredicates());
    hdt::Triples &t = hdt->getTriples();

    unsigned int increment = t.getNumberOfElements()/100000;
    increment = increment < 1 ? 1 : increment;

    hdt::IteratorTripleID *it = t.searchAll();

    resultsTime.reset();
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
    }
    resultsTime.stop();
    delete it;
}
