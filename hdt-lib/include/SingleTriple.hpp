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

namespace hdt {
class TripleID
{
	private:
		unsigned int subject;
		unsigned int object;
		unsigned int predicate;

	public:

		TripleID() {
		}

		TripleID(unsigned int subject, unsigned int predicate, unsigned int object) {
			this->subject = subject;
			this->predicate = predicate;
			this->subject = subject;
		}

		~TripleID() {

		}
		unsigned int getSubject() {
			return subject;
		}
		void setSubject(unsigned int subject) {
			this->subject = subject;
		}

		unsigned  getPredicate() {
			return predicate;
		}

		void setPredicate(unsigned int predicate) {
			this->predicate = predicate;
		}

		unsigned int getObject() {
			return object;
		}

		void setObject(unsigned int object) {
			this->object = object;
		}

		void clear() {
			subject = predicate = object = 0;
		}

		friend std::ostream &operator<<(std::ostream &stream, const TripleID &ts) {
			stream << "TripleString: "<< ts.subject << ", "<< ts.predicate <<", "<< ts.object;

			return stream;
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
			stream << "TripleString: "<< ts.subject << ", "<< ts.predicate <<", "<< ts.object;

			return stream;
		}

		void clear() {
			subject = predicate = object = "";
		}

}; // TripleString{}

 class IteratorTripleID {
 	public:
 		virtual bool hasNext() {
 			return false;
 		}
 		virtual TripleID next() {
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
 			TripleString ts;
 			return ts;
 		}
 };
}

#endif /* SINGLETRIPLE_HPP_ */
