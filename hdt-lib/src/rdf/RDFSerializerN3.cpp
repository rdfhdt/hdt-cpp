/*
 * RDFSerializerN3.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include "RDFSerializerN3.hpp"

using namespace std;

namespace hdt {

RDFSerializerN3::RDFSerializerN3(std::ostream &s, RDFNotation notation)
	: RDFSerializer(notation),
	  output(&s)
	{

}

RDFSerializerN3::RDFSerializerN3(const char *fileName, RDFNotation notation)
	: RDFSerializer(notation),
	  output(new std::ofstream(fileName, ios::binary | ios::out))
{

}

RDFSerializerN3::~RDFSerializerN3() {

}

void RDFSerializerN3::serialize(IteratorTripleString *it, ProgressListener *listener, unsigned int totalTriples)
{
	unsigned int numTriple=0;
	while(it->hasNext()) {
		TripleString *ts = it->next();

		if(!ts->isEmpty()) {
			*output << ts->getSubject() << ' ' << ts->getPredicate() << ' ' << ts->getObject() << " ." << endl;
		}
		numTriple++;

		NOTIFYCOND(listener, "Exporting HDT to RDF", numTriple, totalTriples);
	}
}

}
