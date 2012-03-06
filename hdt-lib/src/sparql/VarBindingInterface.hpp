/*
 * VarBindingBase.h
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#ifndef VARBINDINGBASE_H_
#define VARBINDINGBASE_H_

#include <SingleTriple.hpp>

namespace hdt {


/** Users of the library should use VarBindingID,
  VarBindingInterface for internal use only */
class VarBindingInterface : public VarBindingID {
public:
	virtual ~VarBindingInterface() { }

	virtual unsigned int isOrdered(unsigned int numvar)=0;

	virtual unsigned int estimatedNumResults()=0;
	virtual ResultEstimationType estimationAccuracy()=0;

	virtual bool findNext()=0;
	virtual bool findNext(const char *varName, unsigned int value=0) {
		while(findNext()) {
			if(getVarValue(varName)==value) {
				return true;
			}
		}
		return false;
	}
	virtual bool findNext(unsigned int varIndex, unsigned int value=0) {
		while(findNext()) {
			if(getVarValue(varIndex)==value) {
				return true;
			}
		}
		return false;
	}
	virtual unsigned int getNumVars()=0;
	virtual unsigned int getVarValue(const char *varName)=0;
	virtual unsigned int getVarValue(unsigned int numvar)=0;
	virtual unsigned int getVarIndex(const char *varName) {
		for(int i=0;i<getNumVars();i++) {
			if(strcmp(getVarName(i), varName)==0) {
				return i;
			}
		}
		cout << "Var name: " << varName << " not found" << endl;
		throw "Var name does not exist";
	}
	virtual const char *getVarName(unsigned int numvar)=0;
	virtual void searchVar(unsigned int numvar, unsigned int value)=0;
};

}

#endif /* VARBINDINGBASE_H_ */
