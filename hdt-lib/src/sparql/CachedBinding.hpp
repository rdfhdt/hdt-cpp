/*
 * CachedBinding.h
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#ifndef CACHEDBINDING_H_
#define CACHEDBINDING_H_

#include <algorithm>

#include "VarBindingInterface.hpp"

namespace hdt {
class CachedBinding : public VarBindingInterface {
private:
	VarBindingInterface *child;
	vector<string> varnames;
	vector<vector <unsigned int> > values;
	vector<bool> varSorted;
	unsigned int numRows;
	unsigned int readPos;

public:
	CachedBinding(VarBindingInterface *child) : child(child), readPos(0), numRows(0) {

		for(unsigned int i=0;i<child->getNumVars();i++) {
			varnames.push_back(child->getVarName(i));
			varSorted.push_back(child->isOrdered(i));
		}

		while(child->findNext()) {
			vector<unsigned int> inner;
			for(unsigned int i=0;i<child->getNumVars();i++) {
				inner.push_back(child->getVarValue(i));
			}
			values.push_back(inner);
		}

		// Free memory
		delete child;
	}

	virtual ~CachedBinding() {

	}

	struct Sorter {
		unsigned int field;
		bool operator() (const vector<unsigned int> &i, const vector<unsigned int> &j) { return (i[field]<j[field]);}
	};

	virtual void sortBy(unsigned int numvar){
		if(!varSorted[numvar]) {

			Sorter sorter;
			sorter.field = numvar;

			std::sort(values.begin(), values.end(), sorter);
			varSorted[numvar] = true;
		}
	}

	unsigned int estimatedNumResults(){
		return values.size();
	}
	ResultEstimationType estimationAccuracy(){
		return EXACT;
	}

	bool findNext(){
		if(readPos<values.size()) {
			readPos++;
			return true;
		}
		return false;
	}

	unsigned int getNumVars(){
		return varnames.size();
	}
	unsigned int getVarValue(const char *varName){
		return getVarValue(getVarIndex(varName));
	}
	unsigned int getVarValue(unsigned int numvar){
		return values[readPos][numvar];
	}
	const char *getVarName(unsigned int numvar){
		return varnames[numvar].c_str();
	}
	void searchVar(unsigned int numvar, unsigned int value) {
		throw "Unsupported";
	}

	void goToStart() {
		readPos=0;
	}
};

}

#endif /* CACHEDBINDING_H_ */
