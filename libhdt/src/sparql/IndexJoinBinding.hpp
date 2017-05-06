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
    size_t leftVarValue;
    vector< vector<size_t> > leftOperands, rightOperands;
    //size_t leftCount, rightCount;
	//bool remainingRight;
public:
	IndexJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right);
	virtual ~IndexJoinBinding();
    size_t isOrdered(size_t numvar);

    size_t estimatedNumResults();
	ResultEstimationType estimationAccuracy();

    bool findNext(const char *varName, size_t value=0);

	bool findNext();

    //virtual void findNext(size_t numvar, size_t value=0);
	void goToStart();

    void searchVar(size_t numvar, size_t value);
};

}

#endif /* INDEXJOINBINDING_HPP_ */
