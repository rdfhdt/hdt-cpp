/*
 * PlainDictionary.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PLAINDICTIONARY_H_
#define PLAINDICTIONARY_H_

#include <HDTSpecification.hpp>
#include <Dictionary.hpp>

#include <string.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <ext/hash_map>
   namespace std { using namespace __gnu_cxx; }

namespace hdt {

struct DictionaryEntry {
public:
	unsigned int id;
	const std::string *prefix;
	const std::string *str;

	std::string toString();
	bool static cmpLexicographic(DictionaryEntry *c1, DictionaryEntry *c2);
	bool static cmpID(DictionaryEntry *c1, DictionaryEntry *c2);
};

struct str_cmp {
	bool operator()(const char* s1, const char* s2) const {
	    return strcmp(s1,s2)==0;
	}
};

typedef std::pair<const char*, DictionaryEntry *> DictEntryPair;

#if 0
#include <google/sparse_hash_map>
typedef google::sparse_hash_map<const char *, DictionaryEntry *, std::hash<const char *>, str_cmp> DictEntryHash;
#else
typedef std::hash_map<const char *, DictionaryEntry *, std::hash<const char *>, str_cmp> DictEntryHash;
#endif

typedef DictEntryHash::const_iterator DictEntryIt;
typedef std::hash_map<const char *, std::string *, std::hash<const char*>, str_cmp> PrefixHash;
typedef PrefixHash::iterator PrefixIt;


class PlainDictionary : public Dictionary {
// Private attributes
private:
	std::vector<DictionaryEntry*> predicates;
	std::vector<DictionaryEntry*> subjects_shared;
	std::vector<DictionaryEntry*> subjects_not_shared;
	std::vector<DictionaryEntry*> objects_not_shared;
	DictEntryHash hashSubject;
	DictEntryHash hashPredicate;
	DictEntryHash hashObject;
	PrefixHash prefixes;
	unsigned int mapping;

	//ControlInformation controlInformation;
	HDTSpecification spec;

// Public Interface
public:
	PlainDictionary();
	PlainDictionary(HDTSpecification &spec);
	~PlainDictionary();

	std::string idToString(unsigned int id, TripleComponentRole position);
	unsigned int stringToId(std::string &str, TripleComponentRole position);

	TripleID tripleStringtoTripleID(TripleString &tripleString);
	TripleString tripleIDtoTripleString(TripleID &tripleID);

	unsigned int numberOfElements();

	unsigned int getNsubjects();
	unsigned int getNpredicates();
	unsigned int getNobjects();
	unsigned int getSsubobj();

	unsigned int getMaxID();
	unsigned int getMaxSubjectID();
	unsigned int getMaxPredicateID();
	unsigned int getMaxObjectID();

	void populateHeader(Header &header, string rootNode);
	bool save(std::ostream &output, ControlInformation &ci);
	void load(std::istream &input, ControlInformation &ci);


	void insert(std::string &str, TripleComponentRole position);

	void startProcessing();
	void stopProcessing();

// Private methods
private:
	void insert(std::string entry, DictionarySection pos);

	void split();
	void lexicographicSort();
	void idSort();
	void updateIDs();

	void setPrefixAndString(DictionaryEntry *entry, const std::string str);

	std::vector<DictionaryEntry*> &getDictionaryEntryVector(unsigned int id, TripleComponentRole position);

public:
	unsigned int getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position);
	unsigned int getGlobalId(unsigned int id, DictionarySection position);
	unsigned int getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position);
	unsigned int getLocalId(unsigned int id, TripleComponentRole position);

	void convertMapping(unsigned int mapping);
	void updateID(unsigned int oldid, unsigned int newid, DictionarySection position);



	unsigned int getMapping();

	void dumpStats(std::string &path);
	void dumpSizes(std::ostream &out);
};

}

#endif /* PLAINDICTIONARY_H_ */
