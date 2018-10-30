#ifndef HDTCACHEDINFO_HPP
#define HDTCACHEDINFO_HPP

#include <QString>
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
    size_t maxPredicateCount;
    vector<size_t> predicateCount;

public:
    HDTCachedInfo(hdt::HDT *hdt);

    Color *getPredicateColor(size_t npred);
    size_t getPredicateUsages(size_t predicate);
    size_t getMaxPredicateCount();
    vector<hdt::TripleID> &getTriples();

    void generateGeneralInfo(hdt::ProgressListener *listener=NULL);
    void generateMatrix(hdt::ProgressListener *listener=NULL);

    void save(QString &fileName, hdt::ProgressListener *listener=NULL);
    void load(QString &fileName, hdt::ProgressListener *listener=NULL);
};

#endif // HDTCACHEDINFO_HPP
