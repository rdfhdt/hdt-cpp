#ifndef HDTCACHEDINFO_HPP
#define HDTCACHEDINFO_HPP

#include <HDT.hpp>
#include <HDTListener.hpp>
#include "StopWatch.hpp"

#include "Color.h"


class HDTCachedInfo
{
private:

    hdt::HDT *hdt;
    vector<hdt::TripleID> triples;

    vector<Color> predicateColors;
    unsigned int maxPredicateCount;
    vector<unsigned int> predicateCount;

public:
    HDTCachedInfo(hdt::HDT *hdt);

    Color *getPredicateColor(unsigned int npred);
    unsigned int getPredicateUsages(unsigned int predicate);
    unsigned int getMaxPredicateCount();
    vector<hdt::TripleID> &getTriples();

    void generateInfo(hdt::ProgressListener *listener=NULL);
};

#endif // HDTCACHEDINFO_HPP
