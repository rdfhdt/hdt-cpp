/*
 * UintStream.hpp
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */

#ifndef STREAMELEMENTS_HPP_
#define STREAMELEMENTS_HPP_

#include <vector>
#include <iostream>

namespace hdt {

class IteratorUint {
public:
	virtual bool hasNext() {
		return false;
	}

	virtual unsigned int next() {
		return 0;
	}

	virtual void goToStart() {
	}
};


class StreamElements {

public:
	virtual ~StreamElements() { }

	/**
	 * Adds elementss to the stream
	 *
	 * @param elements
	 *            The elements to be added to the stream
	 */
	virtual void add(IteratorUint &elements)=0;

	/**
	 * Gets the element in a specific position
	 *
	 * @param position
	 *            The position of the element to be returned
	 * @return int
	 */
	virtual unsigned int get(unsigned int position)=0;

	/**
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	virtual unsigned int getNumberOfElements()=0;

	/**
	 * Returns the size of the stream in bytes
	 *
	 * @return int
	 */
	virtual unsigned int size()=0;

	/**
	 * Saves the stream to an OutputStream
	 *
	 * @param output
	 *            The OutputStream to be saved to
	 * @throws IOException
	 */
	virtual void save(std::ostream &output)=0;

	/**
	 * Loads a stream from an InputStream
	 *
	 * @param input
	 *            The InputStream to load from
	 */
	virtual void load(std::istream &input)=0;

	virtual std::string getType()=0;

	static StreamElements *getStream(std::string type);
};

class VectorIterator : public IteratorUint {
private:
	std::vector<unsigned int> &vector;
	unsigned int pos;

public:
	VectorIterator(std::vector<unsigned int> &v) : vector(v), pos(0) {
	}

	bool hasNext(){
		return pos<vector.size();
	}
	unsigned int next(){
		return vector[pos++];
	}
	void goToStart(){
		pos=0;
	}
};

class StreamIterator : public IteratorUint {
private:
	StreamElements *stream;
	unsigned int pos;

public:
	StreamIterator(StreamElements *elements) : stream(elements), pos(0) {
	}

	bool hasNext(){
		return pos<stream->getNumberOfElements();
	}
	unsigned int next(){
		return stream->get(pos++);
	}
	void goToStart() {
		pos=0;
	}
};

}

#endif /* STREAMELEMENTS_HPP_ */
