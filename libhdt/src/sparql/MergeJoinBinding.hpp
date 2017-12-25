/*
 * MergeJoinBinding.hpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#ifndef MERGEJOINBINDING_HPP_
#define MERGEJOINBINDING_HPP_

#include "BaseJoinBinding.hpp"

namespace hdt {

class MergeJoinBinding : public BaseJoinBinding {
	unsigned int s, r;
	vector< vector<unsigned int> > leftOperands, rightOperands;
	unsigned int leftCount, rightCount;
	bool hasMoreOperands;
public:
	MergeJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right);
	virtual ~MergeJoinBinding();

	unsigned int isOrdered(unsigned int numvar);

	unsigned int estimatedNumResults();
	ResultEstimationType estimationAccuracy();
	bool findNext(const char *varName, unsigned int value=0);

	bool findNext();
	//virtual void findNext(unsigned int numvar, unsigned int value=0);
	void goToStart();

	unsigned int getVarValue(unsigned int numvar);
	void searchVar(unsigned int numvar, unsigned int value);
};

}

#endif /* MERGEJOINBINDING_HPP_ */
