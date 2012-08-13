/*
 * QueryProcessor.hpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */

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

	unsigned int getVarIndex(const char *varName) {
        for(unsigned int i=0;i<getNumVars();i++) {
			if(strcmp(getVarName(i), varName)==0) {
				return i;
			}
		}
		cout << "Var name: " << varName << " not found" << endl;
		throw "Var name does not exist";
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

	virtual unsigned int getNumVars() {
		return varID->getNumVars();
	}
	virtual string getVar(unsigned int numvar) {
		unsigned int id = varID->getVarValue(numvar);
		string varName(getVarName(numvar));

		return dict->idToString(id, varRole.find(varName)->second);
	}
	virtual const char *getVarName(unsigned int numvar) {
		return varID->getVarName(numvar);
	}
        virtual void goToStart() {
            return varID->goToStart();
        }
        virtual unsigned int estimatedNumResults() {
            return varID->estimatedNumResults();
        }
};

}

#endif /* QUERYPROCESSOR_HPP_ */
