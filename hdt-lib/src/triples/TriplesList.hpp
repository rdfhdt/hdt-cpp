/*
 * TriplesList.hpp
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
		TripleComponentOrder order;

	public:
		TriplesList();
		virtual ~TriplesList();

		// From Triples

		/**
	     * Returns a vector of triples matching the pattern
	     *
	     * @param pattern
	     * @return
	     */
		IteratorTripleID *search(const TripleID &pattern);

		/**
		 * Calculates the cost to retrieve a specific pattern
	     *
	     * @param triple
	     * @return
	     */
		float cost(const TripleID &triple);

		/**
	     * Returns the number of triples contained
	     *
	     * @return
	     */
		unsigned int getNumberOfElements();

		/**
		 * Returns size in bytes
		 */
		unsigned int size();

		/**
	     * Saves the triples
	     *
	     * @param output
	     * @return
	     */
		bool save(const std::ostream &output);

		/**
	     * Loads triples from a file
	     *
	     * @param input
	     * @return
	     */
		void load(const std::istream &input, const Header &header);

		void load(const ModifiableTriples &input);

		/**
		 * Populates the header
		 *
		 * @param header
		 * @return
		 */
		void populateHeader(const Header &header);

		/**
		 * TODO Define and decide on return type (bool?)
		 */
		void startProcessing();

		/**
		 * TODO Define and decide on return type (bool?)
		 */
		void stopProcessing();

		// From ModifiableTriples

		/**
		 * Adds one or more triples
		 *
		 * @param triples The triples to be inserted
		 * @return boolean
		 */
		bool insert(const TripleID &triple);

		bool insert(const IteratorTripleID *triples);

		/**
		 * Deletes one or more triples according to a pattern
		 *
		 * @param pattern
		 *            The pattern to match against
		 * @return boolean
		 */
		bool remove(TripleID &pattern);

		bool remove(IteratorTripleID *pattern);

		/**
		 * Updates a triple with new components
		 *
		 * @param oldTriple The triple to be replaced
		 * @param newTriple The triple to replace the old one
		 * @return boolean
		 */
		bool edit(TripleID &oldTriple, TripleID &newTriple);

		/**
		 * Sorts the triples based on an order(TripleComponentOrder)
		 *
		 * @param order The order to sort the triples with
		 */
		void sort(TripleComponentOrder order);

		/**
		 * Sets a type of order(TripleComponentOrder)
		 *
		 * @param order The order to set
		 */
		void setOrder(TripleComponentOrder order);


};

} // hdt{}

#endif /* TRIPLESLIST_H_ */
