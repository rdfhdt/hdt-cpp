#ifndef SORTBINDING_HPP
#define SORTBINDING_HPP

#include "VarBindingInterface.hpp"

namespace hdt {

class SortBinding : public VarBindingInterface
{
private:
    vector<string> varnames;
    unsigned int *table;
    char *joinVar;
    unsigned int joinVarPos;
    unsigned int numRows;
    unsigned int numCols;
    unsigned int currentRow;
public:
    // Sort all of the child by var
    SortBinding(char *var, VarBindingInterface *child);
    virtual ~SortBinding();

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

#endif // SORTBINDING_HPP
