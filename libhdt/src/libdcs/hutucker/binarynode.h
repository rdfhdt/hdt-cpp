/* binarytrie.h
 *
 * binary node definition
 *
 */

#ifndef BINARYTRIE_H_INCLUDED
#define BINARYTRIE_H_INCLUDED

#include <BitString.h>
#include <libcdsBasics.h>
using namespace cds_utils;

#include<iostream>
using namespace std;

/** Auxiliar class to handle binary nodes.
 *
 *  @author Carlos Bedregal
 */

typedef struct
{ uint cbits;
  uint code;
} Tcode;

template <class T>
class BNode{
    public:
    int w; //weight
    int pos; //position
    bool type; //0=external, 1=internal
    int endpoint[2];
    BNode* children[2];
    T* obj;

    BNode();
    BNode(int v, int p, int t, T* o);
    ~BNode(){};
    void print();
    void code(int level, Tcode **codes, BitString **tree, bool *bits, int *tpos);
    void setEndpoints(int start, int end);
    void recPrint(int level=0);
};

template <class T>
BNode<T>::BNode(){
    w=-1;
    obj=0;
    pos=-1;
    type=0;
    endpoint[0]=endpoint[1]=-1;
    children[0]=children[1]=0;
}

template <class T>
BNode<T>::BNode(int v, int p, int t, T* o){
    w=v;
    obj=o;
    pos=p;
    type=t;
    endpoint[0]=endpoint[1]=-1;
    children[0]=children[1]=0;
}

template <class T>
void BNode<T>::setEndpoints(int start, int end){
    endpoint[0]=start;
    endpoint[1]=end;
}


template <class T>
void BNode<T>::print(){
    cout<<"@:"<<this<<": w="<<w<<" pos="<<pos<<" type="<<type;
    if(obj) cout<<" obj: "<<*obj;
    cout<<" ["<<endpoint[0]<<","<<endpoint[1]<<"]";
    cout<<endl;
}

template <class T>
void BNode<T>::code(int level, Tcode **codes, BitString **tree, bool *bits, int *tpos)
{
    (*tpos)++;

    if(children[0])
    {
	bits[level] = 0;
        children[0]->code(level+1, codes, tree, bits, tpos);

	bits[level] = 1;
        children[1]->code(level+1, codes, tree, bits, tpos);
    }
    else
    {
	uint code = 0;
	for (int i=1; i<=level; i++) if (bits[i-1]) cds_utils::bitset(&code, level-i);

	(*codes)[pos].code = code;
	(*codes)[pos].cbits = level;
    }

    (*tree)->setBit((*tpos), 1);
    (*tpos)++;
}

template <class T>
void BNode<T>::recPrint(int level){
    cout<<"@L:"<<level<<": w="<<w<<" pos="<<pos<<" type="<<type;
    if(obj) cout<<" ************************* obj: "<<*obj;
    cout<<" ["<<endpoint[0]<<","<<endpoint[1]<<"]";
    cout<<endl;

    if(children[0]){
        children[0]->recPrint(level+1);
        children[1]->recPrint(level+1);
    }
}

#endif // BINARYTRIE_H_INCLUDED
