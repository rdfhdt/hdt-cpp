/*
 * QueryProcessor.hpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */
#include <stdexcept>
#ifndef QUERYPROCESSOR_HPP_
#define QUERYPROCESSOR_HPP_

#include <HDT.hpp>

#include "joins.hpp"

namespace hdt {

class QueryProcessor {
	HDT *hdt;
public:
	QueryProcessor(HDT *hdt);
	virtual ~QueryProcessor();

	VarBindingString *searchJoin(vector<TripleString> &patterns, set<string> &vars);
};


class BasicVarBindingString : public VarBindingString {
private:
	map<string, TripleComponentRole> varRole;
	VarBindingID *varID;
	Dictionary *dict;

    size_t getVarIndex(const char *varName) {
        for(size_t i=0;i<getNumVars();i++) {
			if(strcmp(getVarName(i), varName)==0) {
				return i;
			}
		}
		cerr << "Var name: " << varName << " not found" << endl;
		throw std::runtime_error("Var name does not exist");
	}
public:
	BasicVarBindingString(map<string, TripleComponentRole> &varRole, VarBindingID *varID, Dictionary *dict) :
	varRole(varRole),
	varID(varID),
	dict(dict)
	{

	}

	virtual ~BasicVarBindingString() {
		delete varID;
	}

	virtual bool findNext(){
		return varID->findNext();
	}

    virtual size_t getNumVars() {
		return varID->getNumVars();
	}
	virtual string getVar(size_t numvar) {
		size_t id = varID->getVarValue(numvar);

		return dict->idToString(id, varRole.find(getVarName(numvar))->second);
	}
    virtual const char *getVarName(size_t numvar) {
		return varID->getVarName(numvar);
	}
        virtual void goToStart() {
            return varID->goToStart();
        }
        virtual size_t estimatedNumResults() {
            return varID->estimatedNumResults();
        }
};

}

#endif /* QUERYPROCESSOR_HPP_ */
