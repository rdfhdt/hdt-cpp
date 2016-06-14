/*
 * HDTRW.hpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */

#ifndef HDTRW_HPP_
#define HDTRW_HPP_

#include <HDTSpecification.hpp>
#include <HDT.hpp>

#include <HDTVocabulary.hpp>
#include <RDFParser.hpp>

#include "HDTFactory.hpp"

#include "../util/StopWatch.hpp"

#include "../header/PlainHeader.hpp"

#include "../dictionary/PlainDictionary.hpp"
#include "../dictionary/KyotoDictionary.hpp"

#include "../triples/TriplesList.hpp"
#include "../triples/TriplesKyoto.hpp"
#ifndef WIN32
#include "../triples/TripleListDisk.hpp"
#endif


namespace hdt {

class BasicModifiableHDT : public ModifiableHDT {
private:
	Header *header;
	Dictionary *dictionary;
	ModifiableTriples *triples;
	HDTSpecification spec;
	string fileName;

	void createComponents();

public:
	BasicModifiableHDT();

	BasicModifiableHDT(HDTSpecification &spec);

	virtual ~BasicModifiableHDT();

	Header *getHeader();

	Dictionary *getDictionary();

	Triples *getTriples();

	void loadFromRDF(const char *fileName, string baseUri, RDFNotation notation, ProgressListener *listener = NULL);

	void loadFromHDT(std::istream &input, ProgressListener *listener = NULL);

	void loadFromHDT(const char *fileName, ProgressListener *listener = NULL);

    void mapHDT(const char *fileName, ProgressListener *listener = NULL);

	void saveToRDF(RDFSerializer &serializer, ProgressListener *listener = NULL);

	void saveToHDT(std::ostream &output, ProgressListener *listener = NULL);

	void saveToHDT(const char *fileName, ProgressListener *listener = NULL);

	void loadOrCreateIndex(ProgressListener *listener = NULL);

	void saveIndex(ProgressListener *listener = NULL);

	/*
	 * FROM RDFAccess
	 */

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString *search(const char *subject, const char *predicate, const char *object);

	VarBindingString *searchJoin(vector<TripleString> &patterns, set<string> &vars);

	/**
	 *
	 * @param triples
	 */
	void insert(TripleString &triple);

	void insert(IteratorTripleString *triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	void remove(TripleString &triples);

	void remove(IteratorTripleString *triples);

    bool isIndexed() const {
        return false;
    }
};

}

#endif /* HDTRW_HPP_ */
