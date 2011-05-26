/*
 * SingleTriple.hpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef SINGLETRIPLE_HPP_
#define SINGLETRIPLE_HPP_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace hdt {
class TripleID
{
	private:
		unsigned int subject;
		unsigned int predicate;
		unsigned int object;

	public:

		TripleID() {
		}

		TripleID(unsigned int subject, unsigned int predicate, unsigned int object) {
			this->subject = subject;
			this->predicate = predicate;
			this->object = object;
		} // TripleID()

		~TripleID() {
		}

		unsigned int getSubject() {
			return subject;
		}
		void setSubject(unsigned int subject) {
			this->subject = subject;
		}

		unsigned  getPredicate() {
			return this->predicate;
		}

		void setPredicate(unsigned int predicate) {
			this->predicate = predicate;
		}

		unsigned int getObject() {
			return this->object;
		}

		void setObject(unsigned int object) {
			this->object = object;
		}

		void clear() {
			this->subject = 0;
			this->predicate = 0;
			this->object = 0;
		} //clear()

		friend std::ostream &operator<<(std::ostream &stream, const TripleID &ts) {
			stream << ts.subject << ", "<< ts.predicate <<", "<< ts.object;

			return stream;
		}

		/**
		 * Compares two triples
		 *
		 * @param operand The operand to compare with
		 * @return boolean
		 */
		bool operator==(TripleID &operand) {
			// Subject comparison
			if (this->subject != operand.subject) {
				return false;
			}

			// Object comparison (since subject was successful)
			if (this->object != operand.object) {
				return false;
			}

			// Predicate comparison (since subject and object were successful)
			if (this->predicate != operand.predicate) {
				return false;
			}
			return true;
		} // ==()

		/**
		 * Inverts the comparison of two triples
		 *
		 * @param operand The operand to compare with
		 * @return boolean
		 */
		bool operator!=(TripleID &operand) {
			return !(this->operator==(operand));
		} // !=()

		/**
		 * Compares two triples and returns -1, 0 or 1
		 *
		 * @param other
		 * @return
		 */
		int compare(TripleID &operand) {

			int result = this->subject - operand.subject;

			if(result==0) {
				result = this->predicate - operand.predicate;
				if(result==0) {
					return this->object - operand.object;
				} else {
					return result;
				}
			} else {
				return result;
			}
		}

		/**
		 * Match a triple to a pattern of TripleID. 0 acts as a wildcard
		 *
		 * @param pattern The pattern to match against
		 * @return boolean
		 */
		bool match(TripleID &pattern) {

			// Save triple components
			unsigned int subject = pattern.getSubject();
			unsigned int object = pattern.getObject();
			unsigned int predicate = pattern.getPredicate();

			if (subject == 0 || subject == this->subject) {
				if (object == 0 || object == this->object) {
					if (predicate == 0 || predicate == this->predicate) {
						return true;
					}
				}
			}

			return false;

		} // match()

		/**
		 * Replaces the contents of a triple with the provided replacement
		 *
		 * @param replacement
		 */
		void replace(TripleID &replacement) {

			this->subject = replacement.getSubject();
			this->object = replacement.getObject();
			this->predicate = replacement.getPredicate();

		} // replace()

		/**
		 * Validates the contents of a triple
		 *
		 * @return boolean
		 */
		bool isValid() {
			return !(this->subject == 0 || this->predicate == 0 || this->object == 0);
		}

};

class TripleString
{
	private:
		std::string subject;
		std::string predicate;
		std::string object;

	public:
		TripleString() {

		}

		TripleString(std::string subject, std::string predicate, std::string object) {
			this->subject = subject;
			this->predicate = predicate;
			this->object = object;
		}

		~TripleString() {

		}

		std::string &getSubject() {
			return subject;
		}
		void setSubject(std::string &subject) {
			this->subject = subject;
		}

		std::string &getPredicate() {
			return predicate;
		}
		void setPredicate(std::string &predicate) {
			this->predicate = predicate;
		}

		std::string &getObject() {
			return object;
		}
		void setObject(std::string &object) {
			this->object = object;
		}

		friend std::ostream &operator<<(std::ostream &stream, const TripleString &ts) {
			stream << ts.subject << ", "<< ts.predicate <<", "<< ts.object;

			return stream;
		}

		void clear() {
			subject = predicate = object = "";
		}

		bool isEmpty() {
			return subject == "" && predicate == "" && object == "";
		}

		bool hasEmpty() {
			return subject == "" || predicate == "" || object == "";
		}

		void read(std::string line){
	        size_t pos_a = 0, pos_b;

	        // Reads the subject
	        pos_b = line.find(" ", pos_a);
	        subject = line.substr(pos_a, pos_b - pos_a);
	        if(subject[0]=='?')
	        	subject = "";
	        pos_a = pos_b + 1;

	        // Reads the predicate
	        pos_b = line.find(" ", pos_a);
	        predicate = line.substr(pos_a, pos_b - pos_a);
	        if(predicate[0]=='?')
	        	predicate = "";
	        pos_a = pos_b + 1;

	        // Reads the predicate
	        pos_b = line.find(" ", pos_a);
	        object = line.substr(pos_a, pos_b - pos_a);
	        if(object[0]=='?')
	        	object = "";
	        pos_a = pos_b;
		}

}; // TripleString{}

 class IteratorTripleID {


 	private:
	    /** The TripleID pattern to match against */
	    TripleID pattern;
	    /** The iterator of TripleID */
	    std::vector<TripleID>::iterator iterator;

 	public:
		IteratorTripleID() {
			//TODO
		}

		IteratorTripleID(std::vector<TripleID>::iterator &it, const TripleID &pat)
			: pattern(pat), iterator(it) { }

		~IteratorTripleID() {
			//delete iterator;
			//TODO
		}

		virtual bool hasNext() {
 			return false;
 		}

 		virtual TripleID next() {
 			// TODO: Return a copy or a pointer to a TripleID?
 			TripleID id;
 			return id;
 		}
 };

 class IteratorTripleString {
 	public:
 		virtual bool hasNext() {
 			return false;
 		}
 		virtual TripleString next() {
 			// TODO: Return a copy or a pointer to a TripleString?
 			TripleString ts;
 			return ts;
 		}
 };
}

#endif /* SINGLETRIPLE_HPP_ */
