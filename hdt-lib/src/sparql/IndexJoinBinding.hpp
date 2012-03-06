/*
 * IndexJoinBinding.hpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#ifndef INDEXJOINBINDING_HPP_
#define INDEXJOINBINDING_HPP_

#include "BaseJoinBinding.hpp"

namespace hdt {

class IndexJoinBinding : public BaseJoinBinding {
	unsigned int leftVarValue;
	vector< vector<unsigned int> > leftOperands, rightOperands;
	unsigned int leftCount, rightCount;
	bool remainingRight;
public:
	IndexJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right);
	virtual ~IndexJoinBinding();
	unsigned int isOrdered(unsigned int numvar);

	unsigned int estimatedNumResults();
	ResultEstimationType estimationAccuracy();

	bool findNext(const char *varName, unsigned int value=0);

	bool findNext();

	//virtual void findNext(unsigned int numvar, unsigned int value=0);
	void goToStart();

	void searchVar(unsigned int numvar, unsigned int value);
};

}

#endif /* INDEXJOINBINDING_HPP_ */
