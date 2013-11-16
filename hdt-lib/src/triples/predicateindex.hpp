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

    }

    size_t getNumAppearances(size_t predicate) {
         //return wavelet->rank(predicate, wavelet->getNumberOfElements());
    }

    size_t getAppearance(size_t predicate, size_t appearance) {
        //return wavelet->select(predicate, appearance);
    }

    void save(std::ostream &output, ProgressListener *listener = NULL) {

    }

    void load(std::istream &input, ProgressListener *listener = NULL) {

    }

    size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL) {

    }

    void generate(ProgressListener *listener) {

    }
};


class PredicateIndexArray : public PredicateIndex {
    IntSequence *array;
    //LogSequence2 *predCount;
    BitSequence375 *bitmap;

public:
    PredicateIndexArray(BitmapTriples *triples) : PredicateIndex(triples), array(NULL), bitmap(NULL) {

    }

    virtual ~PredicateIndexArray() {
        if(array!=NULL) {
            delete array;
        }

        if(bitmap!=NULL) {
            delete bitmap;
        }
    }

    size_t getNumPredicates() {
        //return predCount->getNumberOfElements();
        return bitmap->countOnes();
    }

    size_t getNumAppearances(size_t predicate) {
        //cout << "Using bitmap " << bitmap << endl;
        size_t a = bitmap->select1(predicate)-bitmap->select1(predicate-1);
//        size_t b = predCount->get(predicate-1);
//        if(a!=b) {
//            cout << "Error: " << a << " / "<< b <<endl;
//        }
        return a;
    }

    size_t getAppearance(size_t predicate, size_t appearance) {
        // FIXME: Cache last select1 call?
        return array->get(bitmap->select1(predicate)+appearance-1);
    }

    void save(std::ostream &output, ProgressListener *listener = NULL) {
        bitmap->save(output);

        cout << "Saved bitmap " << bitmap <<" / " << bitmap->getNumBits() << " / " << bitmap->countOnes() << endl;

        //predCount->save(output);
        array->save(output);
    }

    void load(std::istream &input, ProgressListener *listener = NULL) {
        bitmap = BitSequence375::load(input);

        cout << "Loaded bitmap " << bitmap <<" / " << bitmap->getNumBits() << " / " << bitmap->countOnes() << endl;

        //predCount = new LogSequence2();
        //predCount->load(input);

        array = IntSequence::getArray(input);
        array->load(input);
    }

    size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL) {
        size_t count = 0;
        bitmap = new BitSequence375();
        count += bitmap->load(&ptr[count], ptrMax, listener);

        cout << "Mapped bitmap " << bitmap <<" / " << bitmap->getNumBits() << " / " << bitmap->countOnes() << endl;

        //predCount = new LogSequence2();
        //count += predCount->load(ptr, ptrMax, listener);

        array = new LogSequence2();
        count += array->load(&ptr[count], ptrMax, listener);

        return count;
    }

    void generate(ProgressListener *listener);
};

}

#endif // PREDICATEINDEX_HPP
