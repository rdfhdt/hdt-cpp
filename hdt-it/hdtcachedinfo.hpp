#ifndef HDTCACHEDINFO_HPP
#define HDTCACHEDINFO_HPP

#include <HDT.hpp>
#include <HDTListener.hpp>
#include "StopWatch.hpp"

#include "Color.h"


class HDTCachedInfo
{
public:
    hdt::HDT *hdt;
    vector<hdt::TripleID> triples;
    vector<unsigned int> predicateCount;
    vector<Color> predicateColors;
    unsigned int numResults;
    unsigned int maxPredicateCount;
    StopWatch resultsTime;

public:
    HDTCachedInfo(hdt::HDT *hdt);

    Color *getPredicateColor(unsigned int npred);

    void loadInfo(hdt::ProgressListener *listener=NULL);
};

#endif // HDTCACHEDINFO_HPP
