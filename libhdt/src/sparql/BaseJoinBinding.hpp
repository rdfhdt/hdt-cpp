/*
 * BaseJoinBinding.hpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */
#include <stdexcept>
#ifndef BASEJOINBINDING_HPP_
#define BASEJOINBINDING_HPP_

#include "VarBindingInterface.hpp"

namespace hdt {

class BaseJoinBinding : public VarBindingInterface {
protected:
	string bindingVarName;
	int bindingVarPosLeft;
	int bindingVarPosRight;
	VarBindingInterface *left, *right;
	vector<int> vars;	   // Specifies the position of the child where to fetch the var.
	vector<bool> varOperand;       // Specifies wether the var is located in the left or right operand.
	vector<string>varnames;  // If not bound, return null.
	//hash_map<string, char> varsByName;

public:
	BaseJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right)
    : bindingVarName(var),
      left(left),
      right(right)
	{
		// First is always the join variable.
		varnames.push_back(var);
		vars.push_back(left->getVarIndex(var));
		varOperand.push_back(0);

		// Add all from left that are not the join variable.
		for(unsigned int i=0;i<left->getNumVars();i++) {
			const char *varName = left->getVarName(i);

			if(strcmp(varName, var)!=0) {
				varnames.push_back(varName);
				vars.push_back(i);
				varOperand.push_back(0);
			}
		}

		// Add all from right that are not the join variable.
		for(unsigned int i=0;i<right->getNumVars();i++) {
			const char *varName = right->getVarName(i);

			if(strcmp(varName, var)!=0) {
				varnames.push_back(varName);
				vars.push_back(i);
				varOperand.push_back(1);
			}
		}

		bindingVarPosLeft = left->getVarIndex(var);
		bindingVarPosRight = right->getVarIndex(var);
	}

	virtual ~BaseJoinBinding() {
		delete left;
		delete right;
	}

	virtual unsigned int isOrdered(unsigned int numvar)=0;

	virtual unsigned int estimatedNumResults()=0;
	virtual ResultEstimationType estimationAccuracy()=0;

	virtual bool findNext()=0;
	virtual bool findNext(const char *varName, unsigned int value=0)=0;

	unsigned int getNumVars() {
		return varnames.size();
	}

	virtual unsigned int getVarValue(const char *varName) {
		return getVarValue(getVarIndex(varName));
	}

	unsigned int getVarValue(unsigned int numvar) {
		if(numvar>vars.size()) {
			throw std::out_of_range("Accessing out of bound variable");
		}
		if(!varOperand[numvar]) {
			// Left operand
			return left->getVarValue(vars[numvar]);
		} else {
			// Right operand
			return right->getVarValue(vars[numvar]);
		}
	}
	const char *getVarName(unsigned int numvar) {
		if(numvar>vars.size()){
			throw std::runtime_error("Variable not available");
		}
		return varnames[numvar].c_str();
	}

	virtual void searchVar(unsigned int numvar, unsigned int value)=0;
};


}

#endif /* BASEJOINBINDING_HPP_ */
