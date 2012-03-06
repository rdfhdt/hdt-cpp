/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <iostream>

using namespace std;

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
            float newLevel = min + level*(max-min)/100;
            child->notifyProgress(newLevel, section);
	}

	void setRange(float min, float max) {
		this->min=min;
		this->max=max;
	}
};

class ConvertProgress : public ProgressListener {

public:
    void notifyProgress(float level, const char *section) {
    	cout << "Progress: " << level << " " << section << endl;
	}

};

int main(int argc, char **argv) {
	ConvertProgress progress;

	IntermediateListener iListener(&progress);

	iListener.setRange(0,30);
	for(int i=0;i<100;i+=2){
		iListener.notifyProgress(i, "Process 1");
	}

	iListener.setRange(30,60);
	for(int i=0;i<100;i+=2){
		iListener.notifyProgress(i, "Process 2");
	}

	iListener.setRange(60,100);
	for(int i=0;i<100;i+=2){
		iListener.notifyProgress(i, "Process 3");
	}
	
}

