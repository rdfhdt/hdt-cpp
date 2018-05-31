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
	//VarBindingInterface *child;
	vector<string> varnames;
    vector<vector <size_t> > values;
	vector<bool> varSorted;
    //size_t numRows;
    size_t readPos;

public:
	CachedBinding(VarBindingInterface *child) : /*child(child), numRows(0),*/ readPos(0) {

        for(size_t i=0;i<child->getNumVars();i++) {
			varnames.push_back(child->getVarName(i));
			varSorted.push_back(child->isOrdered(i));
		}

		while(child->findNext()) {
            vector<size_t> inner;
            for(size_t i=0;i<child->getNumVars();i++) {
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
        size_t field;
        bool operator() (const vector<size_t> &i, const vector<size_t> &j) { return (i[field]<j[field]);}
	};

    virtual void sortBy(size_t numvar){
		if(!varSorted[numvar]) {

			Sorter sorter;
			sorter.field = numvar;

			std::sort(values.begin(), values.end(), sorter);
			varSorted[numvar] = true;
		}
	}

    size_t estimatedNumResults(){
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

    size_t getNumVars(){
		return varnames.size();
	}
    size_t getVarValue(const char *varName){
		return getVarValue(getVarIndex(varName));
	}
    size_t getVarValue(size_t numvar){
		return values[readPos][numvar];
	}
    const char *getVarName(size_t numvar){
		return varnames[numvar].c_str();
	}
    void searchVar(size_t numvar, size_t value) {
		throw std::logic_error("Unsupported");
	}

	void goToStart() {
		readPos=0;
	}
};

}

#endif /* CACHEDBINDING_H_ */
