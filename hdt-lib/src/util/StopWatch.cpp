/*
 * StopWatch.cpp
 *
 *  Created on: 03/03/2011
 *      Author: mck
 */

#include <iostream>
#include <sstream>

#include "StopWatch.hpp"

StopWatch::StopWatch() {
	reset();
}

StopWatch::~StopWatch() {

}

void StopWatch::reset() {
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru);
	gettimeofday(&real1, NULL);

	memcpy(&user1, &ru.ru_utime, sizeof(struct timeval));
	memcpy(&system1, &ru.ru_stime, sizeof(struct timeval));
}

void StopWatch::stop() {
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	gettimeofday(&real2, NULL);

	memcpy(&user2, &ru.ru_utime, sizeof(struct timeval));
	memcpy(&system2, &ru.ru_stime, sizeof(struct timeval));
}


std::string StopWatch::stopUser() {
	stop();
	return getUser();
}

std::string StopWatch::stopSystem() {
	stop();
	return getSystem();
}

std::string StopWatch::stopReal() {
	stop();
	return getReal();
}

std::string StopWatch::getUser() {
	return toHuman(user2.tv_sec-user1.tv_sec, user2.tv_usec-user1.tv_usec);
}

std::string StopWatch::getSystem() {
	return toHuman(system2.tv_sec-user1.tv_sec, system2.tv_usec-system1.tv_usec);
}

std::string StopWatch::getReal() {
	return toHuman(real2.tv_sec-real1.tv_sec, real2.tv_usec-real1.tv_usec);
}

std::ostream &operator<<(std::ostream &stream, StopWatch &sw) {
	stream << sw.stopReal();
	return stream;
}

std::string StopWatch::toHuman(time_t tot_secs, suseconds_t tot_usecs) {
	std::cout << tot_secs << " " << tot_usecs << std::endl;
	unsigned int hours = tot_secs/3600;
	unsigned int mins = (tot_secs/60) % 60;
	unsigned int secs = (tot_secs % 60);
	unsigned int ms = (tot_usecs)/1000;
	unsigned int us = (tot_usecs%1000);

	std::ostringstream out;
	if(hours>0) {
		out << hours << " hour ";
	}
	if(mins>0) {
		out << mins << " min ";
	}
	if(secs>0) {
		out << secs << " sec ";
	}
	if(ms>0) {
		out << ms << " ms ";
	}
	if(us>0){
		out << us << " us";
	}
	return out.str();
}
