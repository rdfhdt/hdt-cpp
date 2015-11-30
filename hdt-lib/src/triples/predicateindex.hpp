#ifndef PREDICATEINDEX_HPP
#define PREDICATEINDEX_HPP

#include <iostream>
#include <stdlib.h>

#include <HDTListener.hpp>


#include "../bitsequence/BitSequence375.h"
#include "../sequence/LogSequence2.hpp"
//#include "../sequence/WaveletSequence.hpp"


namespace hdt {

class BitmapTriples;

class PredicateIndex
{
protected:
    BitmapTriples *triples;
public:
    PredicateIndex(BitmapTriples *triples);

    virtual ~PredicateIndex( ) {
    }

    virtual size_t getNumPredicates()=0;
    virtual size_t getNumAppearances(size_t predicate)=0;
    virtual size_t getAppearance(size_t predicate, size_t appearance)=0;

    virtual void save(std::ostream &output, ProgressListener *listener = NULL)=0;

    virtual void load(std::istream &input, ProgressListener *listener = NULL)=0;

    virtual size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL)=0;

    virtual void generate(ProgressListener *listener)=0;
};


class PredicateIndexWavelet : public PredicateIndex {
protected:
    //WaveletSequence *wavelet;

public:
    PredicateIndexWavelet(BitmapTriples *triples);

    virtual ~PredicateIndexWavelet() {

    }

    size_t getNumPredicates() {
	return 0;
    }
	

    size_t getNumAppearances(size_t predicate) {
         //return wavelet->rank(predicate, wavelet->getNumberOfElements());
	return 0;
    }

    size_t getAppearance(size_t predicate, size_t appearance) {
        //return wavelet->select(predicate, appearance);
	return 0;
    }

    void save(std::ostream &output, ProgressListener *listener = NULL) {

    }

    void load(std::istream &input, ProgressListener *listener = NULL) {

    }

    size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL) {
	return 0;
    }

    void generate(ProgressListener *listener) {

    }
};


class PredicateIndexArray : public PredicateIndex {
    IntSequence *array;
    //LogSequence2 *predCount;
    BitSequence375 *bitmap;
    size_t currpred,currpos;
    //size_t numPredicates;
    BitmapTriples *bitmapTriples;

public:
    PredicateIndexArray(BitmapTriples *triples) : PredicateIndex(triples), array(NULL), bitmap(NULL),currpred(0),bitmapTriples(triples) {

    }

    virtual ~PredicateIndexArray() {
        if(array!=NULL) {
            delete array;
        }

        if(bitmap!=NULL) {
            delete bitmap;
        }
    }


    size_t getNumPredicates();

    size_t getNumAppearances(size_t predicate) {
	//if(currpred!=predicate) {
		currpred = predicate;
		currpos=bitmap->select1(predicate);
	//}
        return currpos-bitmap->select1(predicate-1);
//      return  predCount->get(predicate-1);
    }

    size_t calculatePos(size_t predicate);

    size_t getAppearance(size_t predicate, size_t appearance);

    void save(std::ostream &output, ProgressListener *listener = NULL) {
        bitmap->save(output);
        array->save(output);
    }

    void load(std::istream &input, ProgressListener *listener = NULL) {
        bitmap = BitSequence375::load(input);

        array = IntSequence::getArray(input);
        array->load(input);
    }

    size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

    void generate(ProgressListener *listener);
};

}

#endif // PREDICATEINDEX_HPP
