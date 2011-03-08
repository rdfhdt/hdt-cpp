/*
 * TriplesList.h
 *
 *  Created on: 04/03/2011
 *      Author: billy
 */

#ifndef TRIPLESLIST_H_
#define TRIPLESLIST_H_

#include <Triples.hpp>

namespace hdt {

class TriplesList : public ModifiableTriples {
	private:
		std::vector<TripleID *> arrayOfTriples;

	public:
		TriplesList();
		virtual ~TriplesList();
		/**
		 * Returns a vector of triples matching the pattern
		 *
		 * @param pattern
		 * @return
		 */
		IteratorTripleID *search(const TripleID &pattern);

};

} // hdt{}

#endif /* TRIPLESLIST_H_ */
