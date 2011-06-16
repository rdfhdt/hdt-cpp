/*
 * HDTListener.hpp
 *
 *  Created on: 14/06/2011
 *      Author: mck
 */

#ifndef HDTLISTENER_HPP_
#define HDTLISTENER_HPP_

namespace hdt {

class ProgressListener {
public:
	virtual void notifyProgress(float level, const char *section)=0;
};

}


#endif /* HDTLISTENER_HPP_ */
