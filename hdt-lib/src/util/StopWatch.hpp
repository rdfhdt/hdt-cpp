/*
 * StopWatch.hpp
 *
 *  Created on: 03/03/2011
 *      Author: mck
 */

#ifndef STOPWATCH_HPP_
#define STOPWATCH_HPP_

#ifdef WIN32
#include <windows.h>
#else
# include <sys/time.h>
# include <sys/resource.h>
#endif

#include <string>
#include <iostream>

class StopWatch {
private:

#ifdef WIN32
	LARGE_INTEGER frequency;
	LARGE_INTEGER startCount;
	LARGE_INTEGER endCount;
#else
	struct timeval user1, user2;
	struct timeval system1, system2;
	struct timeval real1, real2;
#endif

public:
	StopWatch();

	void reset();
	void stop();

	unsigned long long getUser();
	unsigned long long getSystem();
	unsigned long long getReal();

	std::string getUserStr();
	std::string getSystemStr();
	std::string getRealStr();

	unsigned long long stopUser();
	unsigned long long stopSystem();
	unsigned long long stopReal();

	std::string stopUserStr();
	std::string stopSystemStr();
	std::string stopRealStr();

	static std::string toHuman(unsigned long long time);

	friend std::ostream &operator<<(std::ostream &stream, StopWatch &sw);
};

#endif /* STOPWATCH_HPP_ */
