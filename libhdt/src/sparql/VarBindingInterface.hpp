/*
 * VarBindingBase.h
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */
#include <stdexcept>
#ifndef VARBINDINGBASE_H_
#define VARBINDINGBASE_H_

#include <SingleTriple.hpp>

namespace hdt {


/** Users of the library should use VarBindingID,
  VarBindingInterface for internal use only */
class VarBindingInterface : public VarBindingID {
public:
	virtual ~VarBindingInterface() { }

    virtual size_t isOrdered(size_t numvar)=0;

    virtual size_t estimatedNumResults()=0;
	virtual ResultEstimationType estimationAccuracy()=0;

	virtual bool findNext()=0;
    virtual bool findNext(const char *varName, size_t value=0) {
		while(findNext()) {
			if(getVarValue(varName)==value) {
				return true;
			}
		}
		return false;
	}
    virtual bool findNext(size_t varIndex, size_t value=0) {
		while(findNext()) {
			if(getVarValue(varIndex)==value) {
				return true;
			}
		}
		return false;
	}
    virtual size_t getNumVars()=0;
    virtual size_t getVarValue(const char *varName)=0;
    virtual size_t getVarValue(size_t numvar)=0;
    virtual size_t getVarIndex(const char *varName) {
		for(int i=0;i<getNumVars();i++) {
			if(strcmp(getVarName(i), varName)==0) {
				return i;
			}
		}
		cerr << "Var name: " << varName << " not found" << endl;
		throw std::runtime_error("Var name does not exist");
	}
    virtual const char *getVarName(size_t numvar)=0;
    virtual void searchVar(size_t numvar, size_t value)=0;
};

}

#endif /* VARBINDINGBASE_H_ */
