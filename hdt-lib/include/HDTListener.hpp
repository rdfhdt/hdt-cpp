/*
 * File: HDTListener.hpp
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

#ifndef HDT_HDTLISTENER_HPP_
#define HDT_HDTLISTENER_HPP_

#include <iostream>
#include <iomanip>
#include <stddef.h>

namespace hdt {

class ProgressListener {
public:
	virtual ~ProgressListener() { }
    void notifyProgress(float level, const char *section) {
        this->notifyProgress(level, level, section);
    }

    virtual void notifyProgress(float task, float level, const char *section)=0;
};

#ifdef WIN32
#undef max
#undef min
#endif


class IntermediateListener : public ProgressListener {
	ProgressListener *child;
	float min, max;
public:
	IntermediateListener(ProgressListener *child)
		: child(child), min(0), max(100) {
	}

	virtual ~IntermediateListener() { }

    virtual void notifyProgress(float task, float level, const char *section) {
		if(child!=NULL){
            float newLevel = min + level*(max-min)/100;
            child->notifyProgress(task, newLevel, section);
		}
	}

    void notifyProgress(float level, const char *section) {
        if(child!=NULL){
            float newLevel = min + level*(max-min)/100;
            child->notifyProgress(level, newLevel, section);
        }
    }

	void setRange(float min, float max) {
		this->min=min;
		this->max=max;
	}
};

class StdoutProgressListener : public ProgressListener {
private:
public:
	virtual ~StdoutProgressListener() { }

    void notifyProgress(float level, const char *section) {
    	std::cerr << "\r " << std::setw( 3 ) << std::setprecision( 5 )<< section << ": " << level << " %                      \r";
		std::cerr.flush();
	}

    void notifyProgress(float task, float level, const char *section) {
    	std::cerr << "\r " << std::setw( 3 ) << std::setprecision( 5 )<< section << ": " << task << " % / " << level << " %                      \r";
                std::cerr.flush();
        }

};

#define NOTIFY(listener, message, number, total) \
    if((listener)!=NULL && total!=0) (listener)->notifyProgress( ((number)*100/(total)), (message));

#define NOTIFYCOND(listener, message, number, total) \
    if((listener)!=NULL && total!=0 && ((number)%50000) == 0) (listener)->notifyProgress( ((number)*100.0/(total)), (message));

#define NOTIFYCOND2(listener, message, counter, number, total) \
    if((listener)!=NULL && total!=0 && ((counter)%50000) == 0) (listener)->notifyProgress( ((number)*100.0/(total)), (message));

#define NOTIFYCOND3(listener, message, counter, total, step) \
    if((listener)!=NULL && total!=0 && ((counter)%(step)) == 0) (listener)->notifyProgress( ((counter)*100.0/(total)), (message));

}

#endif /* HDT_HDTLISTENER_HPP_ */
