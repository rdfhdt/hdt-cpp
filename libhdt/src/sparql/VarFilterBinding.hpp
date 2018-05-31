#include <stdexcept>
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
    for(size_t i=0;i<child->getNumVars();i++) {
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

    size_t isOrdered(size_t numvar) {
	return child->isOrdered(numvar);
    }

    size_t estimatedNumResults() {
	return child->estimatedNumResults();
    }

    ResultEstimationType estimationAccuracy() {
	return child->estimationAccuracy();
    }

    bool findNext() {
	return child->findNext();
    }

    bool findNext(const char *varName, size_t value=0) {
	return child->findNext(varName,value);
    }

    size_t getNumVars() {
	return varNames.size();
    }

    size_t getVarValue(const char *varName) {
	return getVarValue(getVarIndex(varName));
    }

    size_t getVarValue(size_t numvar) {
	if(numvar>varIds.size()) {
	    throw std::runtime_error("No such variable");
	}
	return child->getVarValue(varIds[numvar]);
    }

    const char *getVarName(size_t numvar) {
	if(numvar>varIds.size()) {
	    throw std::runtime_error("No such variable");
	}
	return varNames[numvar].c_str();
    }

    void searchVar(size_t numvar, size_t value) {
	return child->searchVar(varIds[numvar], value);
    }

    void goToStart() {
	child->goToStart();
    }
};

}

#endif // VARFILTERBINDING_HPP
