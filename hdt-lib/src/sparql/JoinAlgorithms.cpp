/*
 * JoinAlgorithms.cpp
 *
 *  Created on: 13/09/2011
 *      Author: mck
 */

#include "JoinAlgorithms.hpp"

namespace hdt {

#define CHECK_COND(a, b) ((a).size()>0 && (a).at(0)=='?' && (a)==(b))

unsigned short getComparisons(TripleString &a, TripleString &b) {
	unsigned short out=0;

	out |= CHECK_COND(a.getSubject(), b.getSubject());
	out |= CHECK_COND(a.getSubject(), b.getPredicate()) << 1;
	out |= CHECK_COND(a.getSubject(), b.getObject()) << 2;

	out |= CHECK_COND(a.getPredicate(), b.getSubject()) << 3;
	out |= CHECK_COND(a.getPredicate(), b.getPredicate()) << 4;
	out |= CHECK_COND(a.getPredicate(), b.getObject()) << 5;

	out |= CHECK_COND(a.getObject(), b.getSubject()) << 6;
	out |= CHECK_COND(a.getObject(), b.getPredicate()) << 7;
	out |= CHECK_COND(a.getObject(), b.getObject()) << 8;

	out |= CHECK_COND(a.getSubject(), a.getPredicate()) << 9;
	out |= CHECK_COND(a.getSubject(), a.getObject()) << 10;
	out |= CHECK_COND(a.getPredicate(), a.getObject()) << 11;

	out |= CHECK_COND(b.getSubject(), b.getPredicate()) << 12;
	out |= CHECK_COND(b.getSubject(), b.getObject()) << 13;
	out |= CHECK_COND(b.getPredicate(), b.getObject()) << 14;

	//dumpComparisons(out);

	return out;
}

bool checkComparisons(TripleID &a, TripleID &b, unsigned short conditions) {

	if( (conditions & (1<<0)) && a.getSubject()!=b.getSubject()) {
		return false;
	}
	if( (conditions & (1<<1)) && a.getSubject()!=b.getPredicate()) {
		return false;
	}
	if( (conditions & (1<<2)) && a.getSubject()!=b.getObject()) {
		return false;
	}

	if( (conditions & (1<<3)) && a.getPredicate()!=b.getSubject()) {
		return false;
	}
	if( (conditions & (1<<4)) && a.getPredicate()!=b.getPredicate()) {
		return false;
	}
	if( (conditions & (1<<5)) && a.getPredicate()!=b.getObject()) {
		return false;
	}

	if( (conditions & (1<<6)) && a.getObject()!=b.getSubject()) {
		return false;
	}
	if( (conditions & (1<<7)) && a.getObject()!=b.getPredicate()) {
		return false;
	}
	if( (conditions & (1<<8)) && a.getObject()!=b.getObject()) {
		return false;
	}

	if( (conditions & (1<<9)) && a.getSubject()!=a.getPredicate()) {
		return false;
	}
	if( (conditions & (1<<10)) && a.getSubject()!=a.getObject()) {
		return false;
	}
	if( (conditions & (1<<11)) && a.getPredicate()!=a.getObject()) {
		return false;
	}

	if( (conditions & (1<<12)) && b.getSubject()!=b.getPredicate()) {
		return false;
	}
	if( (conditions & (1<<13)) && b.getSubject()!=b.getObject()) {
		return false;
	}
	if( (conditions & (1<<14)) && b.getPredicate()!=b.getObject()) {
		return false;
	}

	return true;
}

void dumpComparisons(unsigned short conditions) {
	if( (conditions & (1<<0))) {
		cerr << "SUBJA SUBJB" << endl;
	}
	if( (conditions & (1<<1))) {
		cerr << "SUBJA PREDB" << endl;
	}
	if( (conditions & (1<<2))) {
		cerr << "SUBJA OBJB" << endl;
	}

	if( (conditions & (1<<3))) {
		cerr << "PREDA SUBJB" << endl;
	}
	if( (conditions & (1<<4))) {
		cerr << "PREDA PREDB" << endl;
	}
	if( (conditions & (1<<5))) {
		cerr << "PREDA OBJB" << endl;
	}

	if( (conditions & (1<<6))) {
		cerr << "OBJA SUBJB" << endl;
	}
	if( (conditions & (1<<7))) {
		cerr << "OBJA PREDB" << endl;
	}
	if( (conditions & (1<<8))) {
		cerr << "OBJA OBJB" << endl;
	}

	if( (conditions & (1<<9))) {
		cerr << "SUBJA PREDA" << endl;
	}
	if( (conditions & (1<<10))) {
		cerr << "SUBJA OBJA" << endl;
	}
	if( (conditions & (1<<11))) {
		cerr << "PREDA OBJA" << endl;
	}

	if( (conditions & (1<<12))) {
		cerr << "SUBJB PREDB" << endl;
	}
	if( (conditions & (1<<13))) {
		cerr << "SUBJB OBJB" << endl;
	}
	if( (conditions & (1<<14))) {
		cerr << "PREDB OBJB" << endl;
	}
}

TripleID getFirstPattern(TripleID &pattern, TripleID &tid, unsigned short conditions) {
	TripleID other = pattern;

	if( (conditions & (1<<0))) {
		//cout << "SUBJA SUBJB" << endl;
		other.setSubject(tid.getSubject());
	}
	if( (conditions & (1<<1))) {
		//cout << "SUBJA PREDB" << endl;
		other.setPredicate(tid.getSubject());
	}
	if( (conditions & (1<<2))) {
		//cout << "SUBJA OBJB" << endl;
		other.setObject(tid.getSubject());
	}

	if( (conditions & (1<<3))) {
		//cout << "PREDA SUBJB" << endl;
		other.setSubject(tid.getPredicate());
	}
	if( (conditions & (1<<4))) {
		//cout << "PREDA PREDB" << endl;
		other.setPredicate(tid.getPredicate());
	}
	if( (conditions & (1<<5))) {
		//cout << "PREDA OBJB" << endl;
		other.setObject(tid.getPredicate());
	}

	if( (conditions & (1<<6))) {
		//cout << "OBJA SUBJB" << endl;
		other.setSubject(tid.getObject());
	}
	if( (conditions & (1<<7))) {
		//cout << "OBJA PREDB" << endl;
		other.setPredicate(tid.getObject());
	}
	if( (conditions & (1<<8))) {
		//cout << "OBJA OBJB" << endl;
		other.setObject(tid.getObject());
	}

	return other;
}

TripleID getSecondPattern(TripleID &pattern, TripleID &tid, unsigned short conditions) {
	TripleID other = pattern;

	if( (conditions & (1<<0))) {
		//cout << "SUBJA SUBJB" << endl;
		other.setSubject(tid.getSubject());
	}
	if( (conditions & (1<<1))) {
		//cout << "SUBJA PREDB" << endl;
		other.setSubject(tid.getPredicate());
	}
	if( (conditions & (1<<2))) {
		//cout << "SUBJA OBJB" << endl;
		other.setSubject(tid.getObject());
	}

	if( (conditions & (1<<3))) {
		//cout << "PREDA SUBJB" << endl;
		other.setPredicate(tid.getSubject());
	}
	if( (conditions & (1<<4))) {
		//cout << "PREDA PREDB" << endl;
		other.setPredicate(tid.getPredicate());
	}
	if( (conditions & (1<<5))) {
		//cout << "PREDA OBJB" << endl;
		other.setPredicate(tid.getObject());
	}

	if( (conditions & (1<<6))) {
		//cout << "OBJA SUBJB" << endl;
		other.setObject(tid.getSubject());
	}
	if( (conditions & (1<<7))) {
		//cout << "OBJA PREDB" << endl;
		other.setObject(tid.getPredicate());
	}
	if( (conditions & (1<<8))) {
		//cout << "OBJA OBJB" << endl;
		other.setObject(tid.getObject());
	}

	return other;
}




}
