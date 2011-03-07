/*
 * RDFSerializerN3.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include "RDFSerializerN3.hpp"

using namespace std;

namespace hdt {

RDFSerializerN3::~RDFSerializerN3() {
	// TODO Auto-generated destructor stub
}

void RDFSerializerN3::serialize(IteratorTripleString & it)
{
	while(it.hasNext()) {
		TripleString ts = it.next();

		stream << ts.getSubject() << ' ' << ts.getPredicate() << ' ' << ts.getObject() << " ." << endl;
	}
}

void RDFSerializerN3::endProcessing()
{
}

}
