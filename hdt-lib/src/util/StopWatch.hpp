/*
 * StopWatch.hpp
 *
 *  Created on: 03/03/2011
 *      Author: mck
 */

#ifndef STOPWATCH_HPP_
#define STOPWATCH_HPP_

#include <sys/time.h>
#include <string>
#include <iostream>

class StopWatch {
private:
	//unsigned long ini, end;

	struct timeval user1, user2;
	struct timeval system1, system2;
	struct timeval real1, real2;

	std::string toHuman(time_t tot_secs, suseconds_t tot_usecs);

public:
	StopWatch();
	~StopWatch();

	void reset();
	void stop();

	std::string stopUser();
	std::string stopSystem();
	std::string stopReal();

	std::string getUser();
	std::string getSystem();
	std::string getReal();

	friend std::ostream &operator<<(std::ostream &stream, StopWatch &sw);
};

#endif /* STOPWATCH_HPP_ */
