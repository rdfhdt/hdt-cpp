/*
 * Triples.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
 * TODO: Define me
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Guillermo Rodriguez-Cano:  wileeam@acm.org
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *   Mario Arias:               mario.arias@gmail.com
 *
 * @version $Id$
 *
 */

#ifndef HDT_TRIPLES_
#define HDT_TRIPLES_

#include <HDTEnums.hpp>
#include <SingleTriple.hpp>
#include <Header.hpp>

#include <iostream>
#include <iterator>
#include <vector>

namespace hdt {

class ModifiableTriples;

	class Triples
	{
		public:
			/**
		     * Returns a vector of triples matching the pattern
		     *
		     * @param pattern
		     * @return
		     */
			virtual IteratorTripleID *search(TripleID &pattern)=0;

			/**
			 * Calculates the cost to retrieve a specific pattern
		     *
		     * @param triple
		     * @return
		     */
			virtual float cost(TripleID &triple)=0;

			/**
		     * Returns the number of triples
		     *
		     * @return
		     */
			virtual unsigned int getNumberOfElements()=0;

			/**
			 * Returns size in bytes
			 */
			virtual unsigned int size()=0;

			/**
		     * Saves the triples
		     *
		     * @param output
		     * @return
		     */
			virtual bool save(std::ostream &output)=0;


			virtual void load(ModifiableTriples &input)=0;

			/**
		     * Loads triples from a file
		     *
		     * @param input
		     * @return
		     */
			virtual void load(std::istream &input, Header &header)=0;

			/**
			 * Populates the header
			 *
			 * @param header
			 * @return
			 */
			virtual void populateHeader(Header &header)=0;

			virtual void startProcessing()=0;

			virtual void stopProcessing()=0;
	}; // Triples{}

	class ModifiableTriples : public Triples {
	public:
		/**
			 * Adds one or more triples
			 *
			 * @param triples
			 *            The triples to be inserted
			 * @return boolean
			 */
			virtual bool insert(TripleID &triple)=0;

			virtual bool insert(IteratorTripleID *triples)=0;

			/**
			 * Deletes one or more triples according to a pattern
			 *
			 * @param pattern
			 *            The pattern to match against
			 * @return boolean
			 */
			virtual bool remove(TripleID &pattern)=0;

			virtual bool remove(IteratorTripleID *pattern)=0;

			/**
			 * Updates a triple with new components
			 *
			 * @param oldTriple
			 *            The triple to be replaced
			 * @param newTriple
			 *            The triple to replace the old one
			 * @return boolean
			 */
			virtual bool edit(TripleID &oldTriple, TripleID &newTriple)=0;

			/**
			 * Sorts the triples based on an order(TripleComponentOrder)
			 *
			 * @param order
			 *            The order to sort the triples with
			 */
			virtual void sort(TripleComponentOrder order)=0;

			/**
			 * Sets a type of order(TripleComponentOrder)
			 *
			 * @param order
			 *            The order to set
			 */
			virtual void setOrder(TripleComponentOrder order)=0;

	};


}

#endif /* HDT_TRIPLES_ */

