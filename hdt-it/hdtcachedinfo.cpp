#include "hdtcachedinfo.hpp"
#include "constants.h"

#include <fstream>

HDTCachedInfo::HDTCachedInfo(hdt::HDT *hdt) : hdt(hdt)
{

}

void HDTCachedInfo::generateGeneralInfo(hdt::ProgressListener *listener)
{
    // Count predicates
    unsigned int nPred = hdt->getDictionary()->getNpredicates();

    maxPredicateCount = 0;
    predicateCount.clear();
    predicateCount.resize(nPred+1);

    hdt::Triples *t = hdt->getTriples();
    hdt::TripleID triplePredicate;
    for(int p=1;p<=nPred;p++) {
        triplePredicate.setAll(0, p, 0);
        hdt::IteratorTripleID *predIt = t->search(triplePredicate);

        predicateCount[p] = predIt->estimatedNumResults();

        maxPredicateCount = max(maxPredicateCount, predicateCount[p]);

        delete predIt;
    }

    // Calculate Predicate Colors
    predicateColors.clear();
    predicateColors.resize(nPred);
    ColorRamp2 cr;
    for(unsigned int i=0;i<nPred; i++) {
        cr.apply(&predicateColors[i], i, 0, nPred-1);
    }

    // Go over first 50 results to make sure they are loaded.
    hdt::TripleString pat;
    hdt::IteratorTripleString *it2 = hdt->search(pat);
    for(int i=0;i<50 && it2->hasNext(); i++) {
        it2->next();
    }
    delete it2;
}

void HDTCachedInfo::generateMatrix(hdt::ProgressListener *listener)
{
    // Generate matrix
    hdt::Triples *t = hdt->getTriples();
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

void HDTCachedInfo::save(QString &fileName, hdt::ProgressListener *listener)
{
    std::ofstream out(fileName.toAscii(), ios::binary);
    unsigned int numTriples = triples.size();
    out.write((char *)&numTriples, sizeof(unsigned int));
    out.write((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
    out.close();
}

void HDTCachedInfo::load(QString &fileName, hdt::ProgressListener *listener)
{
    generateGeneralInfo(listener);

    std::ifstream in(fileName.toAscii(), ios::binary);
    if(in.good()) {
        unsigned int numTriples;
        in.read((char *)&numTriples, sizeof(unsigned int));
        triples.resize(numTriples);
        in.read((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
        in.close();
    } else {
        generateMatrix(listener);
        save(fileName, listener);
    }
}
