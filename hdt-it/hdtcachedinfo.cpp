#include "hdtcachedinfo.hpp"
#include "constants.h"

#include <fstream>

HDTCachedInfo::HDTCachedInfo(hdt::HDT *hdt) : hdt(hdt)
{

}

void HDTCachedInfo::generateInfo(hdt::ProgressListener *listener)
{
    cout << "Generating info" << endl;
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
    updatePredicateColors();
}

Color * HDTCachedInfo::getPredicateColor(unsigned int npred)
{
    return &predicateColors[npred];
}

void HDTCachedInfo::save(std::string &fileName, hdt::ProgressListener *listener)
{
    cout << fileName << endl;
    std::ofstream out(fileName.c_str(), ios::binary);
    unsigned int numTriples = triples.size();
    out.write((char *)&numTriples, sizeof(unsigned int));
    out.write((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
    unsigned int numPredicates = predicateCount.size();
    out.write((char *)&numPredicates, sizeof(numPredicates));
    out.write((char *)&maxPredicateCount, sizeof(maxPredicateCount));
    out.write((char *)&predicateCount[0], sizeof(unsigned int)*numPredicates);
    out.write((char *)&resultsTime, sizeof(StopWatch));
    out.close();
}

void HDTCachedInfo::load(std::string &fileName, hdt::ProgressListener *listener)
{
    std::ifstream in(fileName.c_str(), ios::binary);
    if(in.good()) {
        unsigned int numTriples;
        in.read((char *)&numTriples, sizeof(unsigned int));
        cout << "num triples: " << numTriples << endl;
        triples.resize(numTriples);
        in.read((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
        numResults = hdt->getTriples().getNumberOfElements();
        unsigned int numPredicates;
        in.read((char *)&numPredicates, sizeof(numPredicates));
        in.read((char *)&maxPredicateCount, sizeof(maxPredicateCount));
        predicateCount.reserve(numPredicates);
        in.read((char *)&predicateCount[0], sizeof(unsigned int)*numPredicates);
        in.read((char *)&resultsTime, sizeof(StopWatch));
        in.close();
    } else {
        generateInfo(listener);
        save(fileName, listener);
    }

    // Calculate Predicate Colors
    updatePredicateColors();
}

void HDTCachedInfo::updatePredicateColors()
{
    unsigned int nPred = hdt->getDictionary().getNpredicates();
    predicateColors.clear();
    predicateColors.resize(nPred);
    ColorRamp2 cr;
    for(unsigned int i=0;i<nPred; i++) {
	cr.apply(&predicateColors[i], i, 0, nPred-1);
    }
}
