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

class IntermediateListener : public ProgressListener {
	ProgressListener *child;
	float min, max;
public:
	IntermediateListener(ProgressListener *child)
		: child(child), min(0), max(100) {
	}

	virtual void notifyProgress(float level, const char *section) {
		if(child!=NULL){
            float newLevel = min + level*(max-min)/100;
            child->notifyProgress(newLevel, section);
		}
	}

	void setRange(float min, float max) {
		this->min=min;
		this->max=max;
	}
};

#define NOTIFY(listener, message, number, total) \
    if((listener)!=NULL) (listener)->notifyProgress( ((number)*100/(total)), (message));

#define NOTIFYCOND(listener, message, number, total) \
    if((listener)!=NULL && ((number)%5000) == 0) (listener)->notifyProgress( ((number)*100/(total)), (message));

}


#endif /* HDTLISTENER_HPP_ */
