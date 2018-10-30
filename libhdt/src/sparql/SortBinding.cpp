#include "SortBinding.hpp"

namespace hdt {


/*
    vector<string> varnames;
    int table[][];
    char *joinVar;
    size_t joinVarPos;
    size_t numRows;
    size_t numCols;
    size_t currentRow;
    */

int compareRow(const void * a, const void * b)
{
    size_t *pa = (size_t *)a;
    size_t *pb = (size_t *)b;
    return pa[0]-pb[0];
}

SortBinding::SortBinding(char *var, VarBindingInterface *child) {
    // Count number of rows
    if(child->estimationAccuracy()==EXACT) {
	numRows = child->estimatedNumResults();
    } else {
	numRows=0;
	while(child->findNext()) {
	    numRows++;
	}
	child->goToStart();
    }
    numCols = child->getNumVars();

    // Reserve space
    table = new size_t[numRows*numCols];

    // Copy
    size_t row=0;
    while(child->findNext()) {
	for(int i=0;i<numCols;i++) {
	    table[row*numCols+i] = child->getVarValue(i);
	}
	row++;
    }

    // Sort
    //qsort(table, numRows, numCols*sizeof(size_t), compare);
}

SortBinding::~SortBinding(){

}

size_t SortBinding::isOrdered(size_t numvar) {
	throw std::logic_error("Not Implemented");
}

size_t SortBinding::estimatedNumResults() {
	throw std::logic_error("Not Implemented");
}

ResultEstimationType SortBinding::estimationAccuracy() {
	throw std::logic_error("Not Implemented");
}

bool SortBinding::findNext(const char *varName, size_t value) {
	throw std::logic_error("Not Implemented");
}

bool SortBinding::findNext() {
	throw std::logic_error("Not Implemented");
}

//virtual void findNext(size_t numvar, size_t value=0);
void SortBinding::goToStart() {

}

size_t SortBinding::getVarValue(size_t numvar) {
	throw std::logic_error("Not Implemented");
}

void SortBinding::searchVar(size_t numvar, size_t value){

}

}
