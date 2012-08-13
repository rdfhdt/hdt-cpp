#ifndef VARFILTERBINDING_HPP
#define VARFILTERBINDING_HPP

#include "VarBindingInterface.hpp"

namespace hdt {

class VarFilterBinding : public VarBindingInterface
{

private:
    VarBindingInterface *child;
    vector<string> varNames;
    vector<int> varIds;
public:
    VarFilterBinding(VarBindingInterface *child, set<string> neededVars) : child(child) {
    for(unsigned int i=0;i<child->getNumVars();i++) {
	    const char *varName = child->getVarName(i);

	    if(neededVars.size()==0 || neededVars.find(varName)!=neededVars.end()) {
		varNames.push_back(child->getVarName(i));
		varIds.push_back(i);
	    }
	}
    }

    ~VarFilterBinding() {
	delete child;
    }

    unsigned int isOrdered(unsigned int numvar) {
	return child->isOrdered(numvar);
    }

    unsigned int estimatedNumResults() {
	return child->estimatedNumResults();
    }

    ResultEstimationType estimationAccuracy() {
	return child->estimationAccuracy();
    }

    bool findNext() {
	return child->findNext();
    }

    bool findNext(const char *varName, unsigned int value=0) {
	return child->findNext(varName,value);
    }

    unsigned int getNumVars() {
	return varNames.size();
    }

    unsigned int getVarValue(const char *varName) {
	return getVarValue(getVarIndex(varName));
    }

    unsigned int getVarValue(unsigned int numvar) {
	if(numvar>varIds.size()) {
	    throw "No such variable";
	}
	return child->getVarValue(varIds[numvar]);
    }

    const char *getVarName(unsigned int numvar) {
	if(numvar>varIds.size()) {
	    throw "No such variable";
	}
	return varNames[numvar].c_str();
    }

    void searchVar(unsigned int numvar, unsigned int value) {
	return child->searchVar(varIds[numvar], value);
    }

    void goToStart() {
	child->goToStart();
    }
};

}

#endif // VARFILTERBINDING_HPP

