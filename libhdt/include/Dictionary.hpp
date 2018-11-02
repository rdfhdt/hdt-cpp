/*
 * File: Dictionary.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef HDT_DICTIONARY_HPP_
#define HDT_DICTIONARY_HPP_

#include "HDTListener.hpp"
#include "SingleTriple.hpp"
#include "Iterator.hpp"
#include "HDTEnums.hpp"
#include "Header.hpp"
#include "ControlInformation.hpp"
#include "Triples.hpp"

#include <string>
#include <iostream>

namespace hdt {

/**
 * Interface representing the Dictionary part of the HDT representation.
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
    virtual std::string idToString(size_t id, TripleComponentRole role)=0;

    /**
    * Fetch the ID assigned to the supplied string as the triple role.
    * If the ID does not exist, it throws an exception.
    * @param str String to be converted.
    * @param role Triple Role (Subject, Predicate, Object) to be fetched.
    * @return ID of the specified String
    */
    virtual size_t stringToId(const std::string &str, TripleComponentRole role)=0;

    /**
    * Convert a TripleString object to a TripleID, using the dictionary to perform the conversion.
    * If any of the components do not exist in the dictionary, it throws an exception.
    * @param tripleString TripleString to be converted.
    * @return resulting TripleID
    */
    void tripleStringtoTripleID(const TripleString &tripleString, TripleID &tid) {
    	tid.setSubject(stringToId(tripleString.getSubject(), SUBJECT));
    	tid.setPredicate(stringToId(tripleString.getPredicate(), PREDICATE));
    	tid.setObject(stringToId(tripleString.getObject(), OBJECT));
    }

    /**
    * Convert a TripleID object to a TripleString, using the dictionary to perform the conversion.
    * If any of the components do not exist in the dictionary, it throws an exception.
    * @param tripleID TripleID to be converted.
    * @return resultant TripleSTring
    */
    void tripleIDtoTripleString(TripleID &tripleID, TripleString &ts) {
	    ts.setSubject(idToString(tripleID.getSubject(), SUBJECT));
    	ts.setPredicate(idToString(tripleID.getPredicate(), PREDICATE));
    	ts.setObject(idToString(tripleID.getObject(), OBJECT));
    }

    /** Number of total elements of the dictionary
    *
    * @return
    */
    virtual size_t getNumberOfElements() =0;

    /**
     * Returns size in bytes of the overall structure.
     */
    virtual uint64_t size()=0;

    /* Return the number of different subjects of the current dictionary. */
    virtual size_t getNsubjects()=0;

    /* Return the number of diferent predicates of the current dictionary */
    virtual size_t getNpredicates()=0;

    /* Return the number of different objects of the current dictionary */
    virtual size_t getNobjects()=0;

    /* return the number of different literal objects of the current dictionary*/
    virtual size_t getNobjectsLiterals()=0;

    /* return the number of different non-literal objects of the current dictionary*/
    virtual size_t getNobjectsNotLiterals()=0;

    /* Return the number of shared subjects-objects of the current dictionary */
    virtual size_t getNshared()=0;

    /* Return the maximum id assigned to the overall dictionary. */
    virtual size_t getMaxID()=0;

    /* Return the maximum subject ID of the dictionary. */
    virtual size_t getMaxSubjectID()=0;

    /* Return the maximum predicate ID of the dictionary. */
    virtual size_t getMaxPredicateID()=0;

    /* Return the maximum object ID of the dictionary. */
    virtual size_t getMaxObjectID()=0;

    virtual void import(Dictionary *other, ProgressListener *listener=NULL)=0;

    virtual IteratorUCharString *getSubjects()=0;
    virtual IteratorUCharString *getPredicates()=0;
    virtual IteratorUCharString *getObjects()=0;
    virtual IteratorUCharString *getShared()=0;

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
    virtual void save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL)=0;

    /**
    * Load dictionary information from a stream.
    * The dictionary may fetch properties from the supplied ControlInformation.
    * @param input Input stream to read the data.
    * @param ci ControlInformation with the specified properties.
    */
    virtual void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL)=0;

    virtual size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL)=0;

    virtual string getType()=0;
    virtual size_t getMapping()=0;

    virtual void getSuggestions(const char *base, TripleComponentRole role, std::vector<string> &out, int maxResults)=0;

    virtual hdt::IteratorUCharString *getSuggestions(const char *prefix, TripleComponentRole role)=0;
    virtual hdt::IteratorUInt *getIDSuggestions(const char *prefix, TripleComponentRole role)=0;
};

class ModifiableDictionary : public Dictionary {
public:
	virtual ~ModifiableDictionary(){ }

    /**
    * Insert a new entry to the dictionary in the corresponding section according to the role (Subject, Predicate, Object).
    * @param str
    * @param role
    */
    virtual size_t insert(const std::string &str, TripleComponentRole role)=0;

    /**
    * Function to be called before starting inserting entries to the dictionary to perform an initialization.
    */
    virtual void startProcessing(ProgressListener *listener = NULL)=0;

    /**
    * Function to be called right after we are done inserting entries, to perform any possible final cleanup.
    */
    virtual void stopProcessing(ProgressListener *listener = NULL)=0;

};

}

#endif /* HDT_DICTIONARY_HPP_ */
