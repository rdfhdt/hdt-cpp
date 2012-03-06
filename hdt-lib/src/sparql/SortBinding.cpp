#include "SortBinding.hpp"

namespace hdt {


/*
    vector<string> varnames;
    int table[][];
    char *joinVar;
    unsigned int joinVarPos;
    unsigned int numRows;
    unsigned int numCols;
    unsigned int currentRow;
    */

int compareRow(const void * a, const void * b)
{
    unsigned int *pa = (unsigned int *)a;
    unsigned int *pb = (unsigned int *)b;
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
    table = new unsigned int[numRows*numCols];

    // Copy
    unsigned int row=0;
    while(child->findNext()) {
	for(int i=0;i<numCols;i++) {
	    table[row*numCols+i] = child->getVarValue(i);
	}
	row++;
    }

    // Sort
    //qsort(table, numRows, numCols*sizeof(unsigned int), compare);
}

SortBinding::~SortBinding(){

}

unsigned int SortBinding::isOrdered(unsigned int numvar) {

}

unsigned int SortBinding::estimatedNumResults() {

}

ResultEstimationType SortBinding::estimationAccuracy() {

}

bool SortBinding::findNext(const char *varName, unsigned int value) {
}

bool SortBinding::findNext() {

}

//virtual void findNext(unsigned int numvar, unsigned int value=0);
void SortBinding::goToStart() {

}

unsigned int SortBinding::getVarValue(unsigned int numvar) {

}

void SortBinding::searchVar(unsigned int numvar, unsigned int value){

}

}
