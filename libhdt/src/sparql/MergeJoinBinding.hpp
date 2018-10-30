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
    size_t s, r;
    vector< vector<size_t> > leftOperands, rightOperands;
    size_t leftCount, rightCount;
	bool hasMoreOperands;
public:
	MergeJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right);
	virtual ~MergeJoinBinding();

    size_t isOrdered(size_t numvar);

    size_t estimatedNumResults();
	ResultEstimationType estimationAccuracy();
    bool findNext(const char *varName, size_t value=0);

	bool findNext();
    //virtual void findNext(size_t numvar, size_t value=0);
	void goToStart();

    size_t getVarValue(size_t numvar);
    void searchVar(size_t numvar, size_t value);
};

}

#endif /* MERGEJOINBINDING_HPP_ */
