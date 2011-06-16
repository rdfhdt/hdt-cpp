/*
 * Dictionary.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
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

#ifndef HDT_DICTIONARY_
#define HDT_DICTIONARY_

#include <string>
#include <iostream>

#include <HDTListener.hpp>
#include <SingleTriple.hpp>
#include <HDTEnums.hpp>
#include <Header.hpp>
#include <ControlInformation.hpp>

namespace hdt {

/**
 * Intreface representing the Dictionary part of the HDT representation.
 * Provides a means to convert Strings to ID and the other way around.
 */
class Dictionary
{
public:
	virtual ~Dictionary(){ }

	/**
	 * Fetch the string associated to the specified ID as the triple role.
	 * If the ID does not exist, it throws an exception.
	 * @param id ID to be fetched
	 * @param role Triple Role (Subject, Predicate, Object) to be fetched.
	 * @return The string associated to that ID.
	 */
	virtual std::string idToString(unsigned int id, TripleComponentRole role)=0;

	/**
	 * Fetch the ID assigned to the supplied string as the triple role.
	 * If the ID does not exist, it throws an exception.
	 * @param str String to be converted.
	 * @param role Triple Role (Subject, Predicate, Object) to be fetched.
	 * @return ID of the specified String
	 */
	virtual unsigned int stringToId(std::string &str, TripleComponentRole role)=0;

	/**
	 * Convert a TripleString object to a TripleID, using the dictionary to perform the conversion.
	 * If any of the components do not exist in the dictionary, it throws an exception.
	 * @param tripleString TripleString to be converted.
	 * @return resulting TripleID
	 */
	virtual void tripleStringtoTripleID(TripleString &tripleString, TripleID &tid)=0;

	/**
	 * Convert a TripleID object to a TripleString, using the dictionary to perform the conversion.
	 * If any of the components do not exist in the dictionary, it throws an exception.
	 * @param tripleID TripleID to be converted.
	 * @return resultant TripleSTring
	 */
	virtual void tripleIDtoTripleString(TripleID &tripleID, TripleString &ts)=0;

	/** Number of total elements of the dictionary
	 *
	 * @return
	 */
	virtual unsigned int getNumberOfElements()=0;

	/* Return the number of different subjects of the current dictionary. */
	virtual unsigned int getNsubjects()=0;

	/* Return the number of diferent predicates of the current dictionary */
	virtual unsigned int getNpredicates()=0;

	/* Return the number of different objects of the current dictionary */
	virtual unsigned int getNobjects()=0;

	/* Return the number of shared subjects-objects of the current dictionary */
	virtual unsigned int getSsubobj()=0;

	/* Return the maximum id assigned to the overall dictionary. */
	virtual unsigned int getMaxID()=0;

	/* Return the maximum subject ID of the dictionary. */
	virtual unsigned int getMaxSubjectID()=0;

	/* Return the maximum predicate ID of the dictionary. */
	virtual unsigned int getMaxPredicateID()=0;

	/* Return the maximum object ID of the dictionary. */
	virtual unsigned int getMaxObjectID()=0;

	/**
	 * Add to the supplied header all relevant information about the current Dictionary
	 * @param header
	 */
	virtual void populateHeader(Header &header, string rootNode)=0;

	/**
	 * Save the current dictionary to a stream, using the format from the standard.
	 * @param output
	 * @return
	 */
	virtual bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL)=0;

	/**
	 * Load dictionary information from a stream.
	 * The dictionary may fetch properties from the supplied ControlInformation.
	 * @param input Input stream to read the data.
	 * @param ci ControlInformation with the specified properties.
	 */
	virtual void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL)=0;

	/**
	 * Insert a new entry to the dictionary in the corresponding section according to the role (Subject, Predicate, Object).
	 * @param str
	 * @param role
	 */
	virtual void insert(std::string &str, TripleComponentRole role)=0;

	/**
	 * Function to be called before starting inserting entries to the dictionary to perform an initialization.
	 */
	virtual void startProcessing()=0;

	/**
	 * Function to be called right after we are done inserting entries, to perform any possible final cleanup.
	 */
	virtual void stopProcessing()=0;

	virtual string getType()=0;
}; // IDictionary{}

} // dictionary{}

#endif  /* HDT_DICTIONARY_ */
