#include "hdtcachedinfo.hpp"
#include "constants.h"

#include "../../hdt-lib/src/triples/BitmapTriples.hpp"

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

   NOTIFYCOND(listener, "Loading predicates", 0, 100);

	// TODO: Use predicateCount directly
    if(hdt->isIndexed()) {
        hdt::Triples *t = hdt->getTriples();
        hdt::TripleID triplePredicate;
        for(int p=1;p<=nPred;p++) {
#if 1
            predicateCount[p] = t->getNumAppearances(p);
#else
            triplePredicate.setAll(0, p, 0);
            hdt::IteratorTripleID *predIt = t->search(triplePredicate);

            predicateCount[p] = predIt->estimatedNumResults();

            maxPredicateCount = max(maxPredicateCount, predicateCount[p]);

            delete predIt;
#endif
        }
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

    // Iterate over elements of the array to make sure their dictionary entries are loaded.
#if 0
    hdt::TripleString out;
    for(size_t i=0;i<triples.size();i++) {
	hdt->getDictionary()->tripleIDtoTripleString(triples[i], out);
    }
#endif
}

void HDTCachedInfo::generateMatrix(hdt::ProgressListener *listener)
{
    // Generate matrix
    hdt::Triples *t = hdt->getTriples();
    size_t increment = t->getNumberOfElements()/RENDER_NUM_POINTS;
    increment = increment < 1 ? 1 : increment;


// Using InterleavedIterator to jump directly to the triple and get the related information.
    hdt::BTInterleavedIterator it(dynamic_cast<hdt::BitmapTriples *>(t), increment);

    size_t count=0;
    while(it.hasNext()) {
        hdt::TripleID *tid = it.next();
        triples.push_back(*tid);
        //cout << *tid << endl;

        NOTIFYCOND(listener, "Generating Matrix", count, RENDER_NUM_POINTS);
        count++;
        //if((count%100)==0)
            //cout << "Iteration: " << count << endl;
    }

// Using normal iterator that goes through all entries.
//    hdt::IteratorTripleID *it = t->searchAll();
//    size_t count=0;
//    for(size_t i=0;i<t->getNumberOfElements();i+=increment) {
//        it->goTo(i);
//        hdt::TripleID *tid = it->next();
//        triples.push_back(*tid);
//        NOTIFYCOND(listener, "Generating Matrix", i, t->getNumberOfElements());
//        count++;
//        if((count%100)==0)
//            cout << "Iteration: " << count << endl;
//    }
//    delete it;

}

Color * HDTCachedInfo::getPredicateColor(size_t npred)
{
    return &predicateColors[npred];
}

size_t HDTCachedInfo::getPredicateUsages(size_t predicate)
{
    return predicateCount[predicate];
}

size_t HDTCachedInfo::getMaxPredicateCount()
{
    return maxPredicateCount;
}

vector<hdt::TripleID> &HDTCachedInfo::getTriples()
{
    return triples;
}

void HDTCachedInfo::save(QString &fileName, hdt::ProgressListener *listener)
{
	// Only save info of files bigger than 2M triples. Otherwise is fast to create from scratch.
	if(hdt->getTriples()->getNumberOfElements()>2000000) {
	    std::ofstream out(fileName.toLatin1(), ios::binary);
        uint64_t numTriples = triples.size();
        out.write((char *)&numTriples, sizeof(uint64_t));
	    out.write((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
	    out.close();
	}
}

void HDTCachedInfo::load(QString &fileName, hdt::ProgressListener *listener)
{
    generateGeneralInfo(listener);

    std::ifstream in(fileName.toLatin1(), ios::binary);
    if(in.good()) {
        uint64_t numTriples;
        in.read((char *)&numTriples, sizeof(uint64_t));
        triples.resize(numTriples);
        in.read((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
        in.close();
    } else {
        generateMatrix(listener);
        save(fileName, listener);
    }
}
