/*
 * File: BasicHDT.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <time.h>

#include <HDTVersion.hpp>
#include <HDTVocabulary.hpp>
#include <RDFParser.hpp>

#include "../util/StopWatch.hpp"
#include "../util/fileUtil.hpp"

#include "ControlInformation.hpp"
#include "HDTFactory.hpp"
#include "BasicHDT.hpp"
#include "../header/PlainHeader.hpp"
#include "../dictionary/PlainDictionary.hpp"
#include "../dictionary/KyotoDictionary.hpp"
#include "../dictionary/FourSectionDictionary.hpp"

#ifdef HAVE_CDS
#include "../dictionary/LiteralDictionary.hpp"
#endif

#include "../triples/TriplesList.hpp"
#include "../triples/TriplesKyoto.hpp"

#ifndef WIN32
#include "../triples/TripleListDisk.hpp"
#endif

#include "../triples/PlainTriples.hpp"
#include "../triples/BitmapTriples.hpp"
#include "../triples/TripleOrderConvert.hpp"

#include "../third/gzstream.h"

#include "TripleIDStringIterator.hpp"

using namespace std;

namespace hdt {


BasicHDT::BasicHDT() : mappedHDT(NULL), mappedIndex(NULL) {
	createComponents();
}

BasicHDT::BasicHDT(HDTSpecification &spec) : mappedHDT(NULL), mappedIndex(NULL) {
	this->spec = spec;
	createComponents();
}

BasicHDT::~BasicHDT() {
	deleteComponents();

    if(mappedHDT) {
    	delete mappedHDT;
    }
    if(mappedIndex) {
       delete mappedIndex;
    }
}

void BasicHDT::createComponents() {
	// HEADER
	header = new PlainHeader();

	// DICTIONARY

	std::string dictType = spec.getOrEmpty("dictionary.type");


	if(dictType==HDTVocabulary::DICTIONARY_TYPE_FOUR) {
		dictionary = new FourSectionDictionary(spec);
	} else if(dictType==HDTVocabulary::DICTIONARY_TYPE_PLAIN) {
		dictionary = new PlainDictionary(spec);
	} else if(dictType==HDTVocabulary::DICTIONARY_TYPE_LITERAL) {
#ifdef HAVE_CDS
		dictionary = new LiteralDictionary(spec);
#else
		throw std::runtime_error("This version has been compiled without support for this dictionary");
#endif
	} else {
		dictionary = new FourSectionDictionary(spec);
	}

	// TRIPLES
	std::string triplesType = spec.getOrEmpty("triples.type");

	if(triplesType==HDTVocabulary::TRIPLES_TYPE_BITMAP) {
		triples = new BitmapTriples(spec);
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_PLAIN) {
		triples = new PlainTriples(spec);
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST) {
                triples = new TriplesList(spec);
#ifndef WIN32
    } else if (triplesType == HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK) {
		triples = new TripleListDisk();
#endif
	} else {
		triples = new BitmapTriples(spec);
	}
}

void BasicHDT::deleteComponents() {
	if (header != NULL)
		delete header;

	if (dictionary != NULL)
		delete dictionary;

	if (triples != NULL)
		delete triples;
}

Header *BasicHDT::getHeader() {
    return header;
}

Dictionary *BasicHDT::getDictionary() {
    return dictionary;
}

Triples *BasicHDT::getTriples() {
    return triples;
}

IteratorTripleString* BasicHDT::search(const char* subject,	const char* predicate, const char* object) {
	TripleString ts(subject, predicate, object);
	try {
		TripleID tid;
		dictionary->tripleStringtoTripleID(ts, tid);

		// Make sure that all not-empty components arefound in dictionary.
		if( (tid.getSubject()==0 && subject!=NULL && *subject!='\0') ||
				(tid.getPredicate()==0 && predicate!=NULL && *predicate!='\0') ||
				(tid.getObject()==0 && object!=NULL && *object!='\0') ) {
			return new IteratorTripleString();
		}

		IteratorTripleID* iterID = triples->search(tid);
		TripleIDStringIterator* iterator = new TripleIDStringIterator(dictionary, iterID);
		return iterator;
	} catch (std::exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
	return new IteratorTripleString();
}


ModifiableDictionary* BasicHDT::getLoadDictionary() {
	return new PlainDictionary(spec);
	//return new KyotoDictionary(spec);
}

ModifiableTriples* BasicHDT::getLoadTriples() {
	return new TriplesList(spec);
	//return new KyotoTriples(spec);
}


void DictionaryLoader::processTriple(const hdt::TripleString& triple,	unsigned long long pos) {
	//cerr << "Triple String: " << triple << endl;
	dictionary->insert(triple.getSubject(), SUBJECT);
	dictionary->insert(triple.getPredicate(), PREDICATE);
	dictionary->insert(triple.getObject(), OBJECT);
	char str[100];
	if ((listener != NULL) && (count % 100000) == 0) {
		sprintf(str, "Generating Dictionary: %lld K triples processed.", count / 1000);
		listener->notifyProgress(0, str);
	}
	count++;
}

void BasicHDT::loadDictionary(const char* fileName, const char* baseUri, RDFNotation notation, ProgressListener* listener) {

	StopWatch st;
	IntermediateListener iListener(listener);

	// Create temporary dictionary
	ModifiableDictionary *dict = getLoadDictionary();
	dict->startProcessing();

	try {
		NOTIFY(listener, "Loading Dictionary", 0, 100);
		iListener.setRange(0, 80);

		// Load data
		DictionaryLoader dictLoader(dict, &iListener);

		RDFParserCallback *parser = RDFParserCallback::getParserCallback(notation);
        parser->doParse(fileName, baseUri, notation, true, &dictLoader);
		delete parser;

		iListener.setRange(80, 90);
		dict->stopProcessing(&iListener);

		// Convert to final format
		if (dictionary->getType()!=HDTVocabulary::DICTIONARY_TYPE_PLAIN){
			dictionary->import(dict);
			delete dict;
		}
		else{
			dictionary = dict;
		}

#ifndef WIN32
	} catch (char *e) {
		cout << "Catch exception dictionary: " << e << endl;
		delete dict;
		throw e;
#else
	} catch(exception& e) {
		cerr << "caught here??" << endl;
		delete dict;
		throw;
#endif
	}
}

void TriplesLoader::processTriple(const hdt::TripleString& triple, unsigned long long pos) {
	TripleID ti;
	dictionary->tripleStringtoTripleID(triple, ti);
	if (ti.isValid()) {
		triples->insert(ti);
	} else {
		stringstream msg;
		msg << "ERROR: Could not convert triple to IDS! " << endl << triple << endl << ti;
		throw ParseException(msg.str());
	}
	//cerr << "TripleID: " << ti << endl;
	char str[100];
	if ((listener != NULL) && (count % 100000) == 0) {
		sprintf(str, "Generating Triples: %lld K triples processed.", count / 1000);
		listener->notifyProgress(0, str);
	}
	count++;
	if(pos>sizeBytes) {
		sizeBytes = pos;
	}
}

void BasicHDT::loadTriples(const char* fileName, const char* baseUri, RDFNotation notation, ProgressListener* listener) {
	// Generate Triples
	ModifiableTriples* triplesList = new TriplesList(spec);
	//ModifiableTriples *triplesList = new TriplesKyoto(spec);
	//ModifiableTriples *triplesList = new TripleListDisk();
	StopWatch st;
	IntermediateListener iListener(listener);
	try {
		NOTIFY(listener, "Loading Triples", 0, 100);
		iListener.setRange(0, 60);

		triplesList->startProcessing(&iListener);

		TriplesLoader tripLoader(dictionary, triplesList, &iListener);

		RDFParserCallback *pars = RDFParserCallback::getParserCallback(
				notation);
		pars->doParse(fileName, baseUri, notation, true, &tripLoader);
		delete pars;
		header->insert("_:statistics", HDTVocabulary::ORIGINAL_SIZE, tripLoader.getSize());
		triplesList->stopProcessing(&iListener);

		// SORT & Duplicates
		string ord = spec.getOrEmpty("triplesOrder");

		TripleComponentOrder order = parseOrder(
				ord.c_str());
		if (order == Unknown) {
			order = SPO;
		}

		iListener.setRange(80, 85);
		triplesList->sort(order, &iListener);

		iListener.setRange(85, 90);
		triplesList->removeDuplicates(&iListener);
	} catch (const char *e) {
		cout << "Catch exception triples" << e << endl;
		delete triplesList;
		throw e;
	} catch (std::exception& e) {
		// cerr << "Catch exception triples" << e << endl;
		delete triplesList;
		throw;
	}
	if (triples->getType() == triplesList->getType()) {
		delete triples;
		triples = triplesList;
	} else {
		iListener.setRange(90, 100);
		try {
			triples->load(*triplesList, &iListener);
		} catch (std::exception& e) {
			delete triplesList;
			throw;
		}
		delete triplesList;
	}

	//cerr << triples->getNumberOfElements() << " triples added in " << st << endl << endl;
}

void BasicHDT::fillHeader(const string& baseUri) {
	string formatNode = "_:format";
	string dictNode = "_:dictionary";
	string triplesNode = "_:triples";
	string statisticsNode = "_:statistics";
	string publicationInfoNode = "_:publicationInformation";

	uint64_t origSize = header->getPropertyLong(statisticsNode.c_str(), HDTVocabulary::ORIGINAL_SIZE.c_str());

	header->clear();

	// BASE
	header->insert(baseUri, HDTVocabulary::RDF_TYPE, HDTVocabulary::HDT_DATASET);

	// VOID
	header->insert(baseUri, HDTVocabulary::RDF_TYPE, HDTVocabulary::VOID_DATASET);
	header->insert(baseUri, HDTVocabulary::VOID_TRIPLES, triples->getNumberOfElements());
	header->insert(baseUri, HDTVocabulary::VOID_PROPERTIES, dictionary->getNpredicates());
	header->insert(baseUri, HDTVocabulary::VOID_DISTINCT_SUBJECTS, dictionary->getNsubjects());
	header->insert(baseUri, HDTVocabulary::VOID_DISTINCT_OBJECTS, dictionary->getNobjects());
	// TODO: Add more VOID Properties. E.g. void:classes

	// Structure
	header->insert(baseUri, HDTVocabulary::HDT_STATISTICAL_INFORMATION,	statisticsNode);
	header->insert(baseUri, HDTVocabulary::HDT_PUBLICATION_INFORMATION,	publicationInfoNode);
	header->insert(baseUri, HDTVocabulary::HDT_FORMAT_INFORMATION, formatNode);
	header->insert(formatNode, HDTVocabulary::HDT_DICTIONARY, dictNode);
	header->insert(formatNode, HDTVocabulary::HDT_TRIPLES, triplesNode);

	// Dictionary
	dictionary->populateHeader(*header, dictNode);

	// Triples
	triples->populateHeader(*header, triplesNode);

	// Sizes
	header->insert(statisticsNode, HDTVocabulary::ORIGINAL_SIZE, origSize);
	header->insert(statisticsNode, HDTVocabulary::HDT_SIZE, getDictionary()->size() + getTriples()->size());

	// Current time
	time_t now;
	char date[40];
	time(&now);
	struct tm* today = localtime(&now);
	strftime(date, 40, "%Y-%m-%dT%H:%M:%S%z", today);
	char *tzm = date+strlen(date) - 2;
	memmove(tzm+1, tzm, 3);
	*tzm = ':';
	header->insert(publicationInfoNode, HDTVocabulary::DUBLIN_CORE_ISSUED, date);
}

void BasicHDT::loadFromRDF(const char *fileName, string baseUri, RDFNotation notation, ProgressListener *listener)
{
	try {
		// Make sure that URI starts and ends with <>
		if(baseUri.at(0)!='<')
			baseUri = '<'+baseUri;
		if(baseUri.at(baseUri.length()-1)!='>')
			baseUri.append(">");

		IntermediateListener iListener(listener);

		iListener.setRange(0,50);
		loadDictionary(fileName, baseUri.c_str(), notation, &iListener);

		iListener.setRange(50,99);
		loadTriples(fileName, baseUri.c_str(), notation, &iListener);

		fillHeader(baseUri);

	}catch (std::exception& e) {
		cerr << "Catch exception load: " << e.what() << endl;
		deleteComponents();
		createComponents();
		throw;
#ifndef WIN32
    } catch (char *e) {
		cout << "Catch exception load: " << e << endl;
		deleteComponents();
		createComponents();
		throw e;
#endif
	}
}

void BasicHDT::addDictionaryFromHDT(const char *fileName, ModifiableDictionary *dict, ProgressListener *listener) {
        cerr << fileName << endl;
        BasicHDT hdt;
        hdt.mapHDT(fileName, listener);

        Dictionary *otherDict = hdt.getDictionary();

        char str[100];

        cerr << endl << "Load dictionary from " << fileName << endl;
        for(long long int i=0;i<otherDict->getNsubjects();i++) {
                dict->insert(otherDict->idToString(i+1, SUBJECT), SUBJECT);

                if ((listener != NULL) && (i % 100000) == 0) {
                        sprintf(str, "%lld subjects added.", i);
                        listener->notifyProgress((i*100)/otherDict->getNsubjects(), str);
                }
        }

        for(long long int i=0;i<otherDict->getNpredicates();i++) {
                dict->insert(otherDict->idToString(i+1, PREDICATE), PREDICATE);
        }

        for(long long int i=0;i<otherDict->getNobjects();i++) {
                dict->insert(otherDict->idToString(i+1, OBJECT), OBJECT);

                if ((listener != NULL) && (i % 100000) == 0) {
                        sprintf(str, "%lld objects added.", i);
                        listener->notifyProgress((i*100)/otherDict->getNobjects(), str);
                }
        }
}


void BasicHDT::loadDictionaryFromHDTs(const char** fileName, size_t numFiles, const char* baseUri, ProgressListener* listener) {

        StopWatch st;
        IntermediateListener iListener(listener);

        // Create temporary dictionary
       	ModifiableDictionary *dict = getLoadDictionary();
       	dict->startProcessing();
        try {
        	NOTIFY(listener, "Loading Dictionary", 0, 100);
        	iListener.setRange(0, 80);

        	for(size_t i=0;i<numFiles;i++) {
        		addDictionaryFromHDT(fileName[i], dict, &iListener);
        	}

        	iListener.setRange(80, 90);
        	dict->stopProcessing(&iListener);

        	// Convert to final format
        	dictionary->import(dict);

        	delete dict;
        } catch (std::exception& e) {
        	cerr << "Catch exception dictionary: " << e.what() << endl;
        	delete dict;
        	throw;
#ifndef WIN32
        } catch (char *e) {
        	cout << "Catch exception dictionary: " << e << endl;
        	delete dict;
        	throw e;
#endif
        }
}

void BasicHDT::loadTriplesFromHDTs(const char** fileNames, size_t numFiles, const char* baseUri, ProgressListener* listener) {
	// Generate Triples
	ModifiableTriples* triplesList = new TriplesList(spec);
	//ModifiableTriples *triplesList = new TriplesKyoto(spec);
	//ModifiableTriples *triplesList = new TripleListDisk();
	StopWatch st;
	IntermediateListener iListener(listener);
	try {
		NOTIFY(listener, "Loading Triples", 0, 100);
		iListener.setRange(0, 60);

		triplesList->startProcessing(&iListener);

		TriplesLoader tripLoader(dictionary, triplesList, &iListener);

		// FIXME: Import from files

		uint64_t totalOriginalSize=0;
		BasicHDT hdt;

		for(size_t i=0;i<numFiles;i++) {
			const char *fileName = fileNames[i];
	        cerr << endl << "Load triples from " << fileName << endl;
	        hdt.mapHDT(fileName);
	        Dictionary *dict = hdt.getDictionary();

	        // Create mapping arrays
            size_t nsubjects = dict->getNsubjects();
	        LogSequence2 subjectMap(bits(dictionary->getNsubjects()), nsubjects);
	        subjectMap.resize(nsubjects);
          for(size_t i=0;i<nsubjects;i++) {
	        	const string str = dict->idToString(i+1, SUBJECT);
            size_t newid = dictionary->stringToId(str, SUBJECT);
	        	subjectMap.set(i, newid);
	        }

            size_t npredicates = dict->getNpredicates();
	        LogSequence2 predicateMap(bits(dictionary->getNpredicates()), npredicates);
	        predicateMap.resize(npredicates);
          for(size_t i=0;i<npredicates;i++) {
	        	const string str = dict->idToString(i+1, PREDICATE);
            size_t newid = dictionary->stringToId(str, PREDICATE);
	        	predicateMap.set(i, newid);
	        }

            size_t nobjects = dict->getNobjects();
	        LogSequence2 objectMap(bits(dictionary->getNobjects()), nobjects);
	        objectMap.resize(nobjects);
          for(size_t i=0;i<nobjects;i++) {
	        	const string str = dict->idToString(i+1, OBJECT);
            size_t newid = dictionary->stringToId(str, OBJECT);
	        	objectMap.set(i, newid);
	        }

	        totalOriginalSize += hdt.getHeader()->getPropertyLong("_:statistics", HDTVocabulary::ORIGINAL_SIZE.c_str());

	        size_t numtriples = hdt.getTriples()->getNumberOfElements();
	        IteratorTripleID *it = hdt.getTriples()->searchAll();

	        TripleID newTid;
	        char str[100];
	        long long int j = 0;
	        while(it->hasNext()) {
	        	TripleID *tid = it->next();

	        	newTid.setAll(
                        subjectMap.get(tid->getSubject()-1),
                        predicateMap.get(tid->getPredicate()-1),
                        objectMap.get(tid->getObject()-1)
	        			);

	        	triplesList->insert(newTid);

	        	if ((listener != NULL) && (j % 100000) == 0) {
	        		sprintf(str, "%lld triples added.", j);
	        		listener->notifyProgress((j*100)/numtriples, str);
	        	}
	            j++;
	        }
	        delete it;
		}

		triplesList->stopProcessing(&iListener);

		// SORT & Duplicates
		string ord = spec.getOrEmpty("triplesOrder");

		TripleComponentOrder order = parseOrder(ord.c_str());
		if (order == Unknown) {
			order = SPO;
		}

		iListener.setRange(80, 85);
		triplesList->sort(order, &iListener);

		iListener.setRange(85, 90);
		triplesList->removeDuplicates(&iListener);

		header->insert("_:statistics", HDTVocabulary::ORIGINAL_SIZE, totalOriginalSize);

#ifndef WIN32
	} catch (char *e) {
		cout << "Catch exception triples" << e << endl;
		delete triplesList;
		throw;
#else
	} catch (std::exception& e) {
		// cerr << "Catch exception triples" << e << endl;
		delete triplesList;
		throw;
#endif
	}
	if (triples->getType() == triplesList->getType()) {
		delete triples;
		triples = triplesList;
	} else {
		iListener.setRange(90, 100);
		try {
			triples->load(*triplesList, &iListener);
		} catch (std::exception& e) {
			delete triplesList;
			throw;
		}
		delete triplesList;
	}

	//cerr << triples->getNumberOfElements() << " triples added in " << st << endl << endl;

}

void BasicHDT::loadFromSeveralHDT(const char **fileNames, size_t numFiles, string baseUri, ProgressListener *listener)
{
	try {
		// Make sure that URI starts and ends with <>
		if(baseUri.at(0)!='<')
			baseUri = '<'+baseUri;
		if(baseUri.at(baseUri.length()-1)!='>')
			baseUri.append(">");

		IntermediateListener iListener(listener);

		iListener.setRange(0,50);
		loadDictionaryFromHDTs(fileNames, numFiles, baseUri.c_str(), &iListener);

		iListener.setRange(50,99);
		loadTriplesFromHDTs(fileNames, numFiles, baseUri.c_str(), &iListener);

		fillHeader(baseUri);

	}catch (std::exception& e) {
		// cerr << "Catch exception load: " << e << endl;
		deleteComponents();
		createComponents();
		throw;
#ifndef WIN32
    } catch (char *e) {
		cout << "Catch exception load: " << e << endl;
		deleteComponents();
		createComponents();
		throw e;
#endif
	}
}



void BasicHDT::saveToRDF(RDFSerializer &serializer, ProgressListener *listener)
{
	IteratorTripleString *it = search("", "", "");
  serializer.serialize(it, listener, getTriples()->getNumberOfElements());
	delete it;
}

void BasicHDT::loadFromHDT(const char *fileName, ProgressListener *listener) {
	this->fileName = fileName;

	DecompressStream stream(fileName);
	istream *in = stream.getStream();
	if(!in->good()){
		throw std::runtime_error("Error opening file to load HDT.");
	}
	this->loadFromHDT(*in, listener);
	stream.close();
}

void BasicHDT::loadHeader(const char *fileName, ProgressListener *listener)
{
	this->fileName = fileName;

		DecompressStream stream(fileName);
		istream *input = stream.getStream();
		if(!input->good()){
			throw std::runtime_error("Error opening file to load HDT.");
		}
    try {
	ControlInformation controlInformation;
	IntermediateListener iListener(listener);

	// Load Global ControlInformation.
	controlInformation.load(*input);
	std::string hdtFormat = controlInformation.getFormat();
	if(hdtFormat!=HDTVocabulary::HDT_CONTAINER) {
		throw std::runtime_error("This software (v" + std::string(HDT_VERSION) + ".x.x) cannot open this version of HDT File (" + hdtFormat + ")");
	}

	// Load header
	iListener.setRange(0,5);
	controlInformation.load(*input);
	delete header;
	header = HDTFactory::readHeader(controlInformation);
	header->load(*input, controlInformation, &iListener);
    } catch (std::exception& e) {
        // cerr << "Exception loading HDT: " << ex;
        deleteComponents();
        createComponents();
        throw;
    }
    stream.close();
}

void BasicHDT::loadFromHDT(std::istream & input, ProgressListener *listener)
{
    try {
	ControlInformation controlInformation;
	IntermediateListener iListener(listener);

	// Load Global ControlInformation.
	controlInformation.load(input);
	std::string hdtFormat = controlInformation.getFormat();
  if(hdtFormat!=HDTVocabulary::HDT_CONTAINER) {
		throw std::runtime_error("This software (v" + std::string(HDT_VERSION) + ".x.x) cannot open this version of HDT File (" + hdtFormat + ")");
	}

	// Load header
	iListener.setRange(0,5);
	controlInformation.load(input);
	delete header;
	header = HDTFactory::readHeader(controlInformation);
	header->load(input, controlInformation, &iListener);

	//Load Dictionary.
	iListener.setRange(5, 60);
	controlInformation.load(input);
	delete dictionary;
	dictionary = HDTFactory::readDictionary(controlInformation);
	dictionary->load(input, controlInformation, &iListener);

	// Load Triples
	iListener.setRange(60,100);
	controlInformation.load(input);
	delete triples;
	triples = HDTFactory::readTriples(controlInformation);
	triples->load(input, controlInformation, &iListener);
    } catch (std::exception& e) {
        // cerr << "Exception loading HDT: " << ex;
        deleteComponents();
        createComponents();
        throw;
#ifndef WIN32
    } catch (char *ex) {
    	cout << "Exception loading HDT: " << ex;
    	deleteComponents();
        createComponents();
        throw ex;
#endif
    }
}

/**
 * Load an HDT from a file, using memory mapping
 * @param input
 */
void BasicHDT::mapHDT(const char *fileNameChar, ProgressListener *listener) {

	IntermediateListener iListener(listener);

    std::string fileStr(fileNameChar);
    size_t pos = fileStr.find_last_of(".");
    std::string suffix = fileStr.substr(pos + 1);

    if( suffix == "gz") {
        #ifdef HAVE_LIBZ
            this->fileName.assign(fileStr.substr(0, pos));
            ifstream test(fileName.c_str());
            if(test.good()) {
               test.close();
            } else {
                test.close();

                iListener.setRange(0,80);
                fileUtil::decompress(fileNameChar, fileName.c_str(), &iListener);
                iListener.setRange(80,100);
            }
        #else
            throw std::runtime_error("Support for GZIP was not compiled in this version. Please decompress the file before opening it.");
        #endif
    } else {
        this->fileName.assign(fileNameChar);
    }

    // Clean previous
    if(mappedHDT!=NULL) {
        delete mappedHDT;
    }

    mappedHDT = new FileMap(fileName.c_str());

    unsigned char *ptr = mappedHDT->getPtr();
    size_t mappedSize = mappedHDT->getMappedSize();

    // Load
    this->loadMMap(ptr, ptr+mappedSize, &iListener);
}

size_t BasicHDT::loadMMap(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener) {
	size_t count=0;
    ControlInformation controlInformation;
    IntermediateListener iListener(listener);

    // Load Global ControlInformation
    count+=controlInformation.load(&ptr[count], ptrMax);
    std::string hdtFormat = controlInformation.getFormat();
    if(hdtFormat!=HDTVocabulary::HDT_CONTAINER) {
    	throw std::runtime_error("This software (v" + std::string(HDT_VERSION) + ".x.x) cannot open this version of HDT File (" + hdtFormat + ")");
    }

    // Load Header
    iListener.setRange(0,5);
    controlInformation.load(&ptr[count], ptrMax);
    delete header;
    header = HDTFactory::readHeader(controlInformation);
    count+= header->load(&ptr[count], ptrMax, &iListener);

	// Load dictionary
    iListener.setRange(5, 60);
    controlInformation.load(&ptr[count], ptrMax);
    delete dictionary;
    dictionary = HDTFactory::readDictionary(controlInformation);
    count += dictionary->load(&ptr[count], ptrMax, &iListener);

	// Load triples
    iListener.setRange(60,100);
    controlInformation.load(&ptr[count], ptrMax);
    delete triples;
    triples = HDTFactory::readTriples(controlInformation);
    count += triples->load(&ptr[count], ptrMax,  &iListener);

	return count;
}

size_t BasicHDT::loadMMapIndex(ProgressListener *listener) {
    // Clean previous index
    if(mappedIndex!=NULL) {
        delete mappedIndex;
    }

    // Get path
    string indexFile(fileName + HDTVersion::get_index_suffix("-"));

    mappedIndex = new FileMap(indexFile.c_str());

    unsigned char *ptr = mappedIndex->getPtr();
    size_t mappedSize = mappedIndex->getMappedSize();

    // Load index
    return triples->loadIndex(ptr, ptr+mappedSize, listener);
}

void BasicHDT::saveToHDT(const char *fileName, ProgressListener *listener)
{
    try {
        ofstream out(fileName, ios::binary | ios::out | ios::trunc);
        if(!out.good()){
            throw std::runtime_error("Error opening file to save HDT.");
        }
        this->fileName = fileName;
        this->saveToHDT(out, listener);
        // Do not create index by default
        // this->saveIndex(listener);
        out.close();
    } catch (std::exception& e) {
        throw;
    }

    this->fileName = fileName;
}

void BasicHDT::saveToHDT(std::ostream & output, ProgressListener *listener)
{
	ControlInformation controlInformation;
	IntermediateListener iListener(listener);

	controlInformation.clear();
	controlInformation.setType(GLOBAL);
	controlInformation.setFormat(HDTVocabulary::HDT_CONTAINER);
	controlInformation.save(output);

	controlInformation.clear();
    controlInformation.setType(HEADER);
	iListener.setRange(0,5);
	header->save(output, controlInformation, &iListener);

	controlInformation.clear();
    controlInformation.setType(DICTIONARY);
	iListener.setRange(5,70);
	dictionary->save(output, controlInformation, &iListener);

	controlInformation.clear();
    controlInformation.setType(TRIPLES);
	iListener.setRange(70,100);
	triples->save(output, controlInformation, &iListener);
}

void BasicHDT::loadOrCreateIndex(ProgressListener *listener) {

	string indexname = this->fileName + HDTVersion::get_index_suffix("-");

	ifstream in(indexname.c_str(), ios::binary);
	// backward compatibility
	if(!in.good()) {
		 indexname = this->fileName+".index";
		 in.open(indexname.c_str(), ios::binary);
	}
	if(in.good()) {
        if(mappedHDT) {
            // Map
            this->loadMMapIndex(listener);
        } else {
            // Read from file
            ControlInformation ci;
            ci.load(in);
            triples->loadIndex(in, ci, listener);
        }
        in.close();
	} else {
        IntermediateListener iListener(listener);
        iListener.setRange(0,90);
        triples->generateIndex(&iListener);

        iListener.setRange(90,100);
        this->saveIndex(&iListener);
    }
}

void BasicHDT::saveIndex(ProgressListener *listener) {
	if(this->fileName.size()==0) {
		cerr << "Cannot save Index if the HDT is not saved" << endl;
		return;
	}

	string indexname = this->fileName + HDTVersion::get_index_suffix("-");
	ofstream out(indexname.c_str(), ios::binary);
	ControlInformation ci;
	triples->saveIndex(out, ci, listener);
	out.close();
}

}
