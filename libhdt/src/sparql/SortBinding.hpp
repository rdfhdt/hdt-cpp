#ifndef SORTBINDING_HPP
#define SORTBINDING_HPP

#include "VarBindingInterface.hpp"

namespace hdt {

class SortBinding : public VarBindingInterface
{
private:
    vector<string> varnames;
    size_t *table;
    //char *joinVar;
    //size_t joinVarPos;
    size_t numRows;
    size_t numCols;
    //size_t currentRow;
public:
    // Sort all of the child by var
    SortBinding(char *var, VarBindingInterface *child);
    virtual ~SortBinding();

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

#endif // SORTBINDING_HPP
