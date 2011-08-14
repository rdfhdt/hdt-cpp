/*
 * StopWatch.cpp
 *
 *  Created on: 03/03/2011
 *      Author: mck
 */

#include <iostream>
#include <sstream>

#include <string.h> // For memcpy()

#include "StopWatch.hpp"

StopWatch::StopWatch() {
#ifdef WIN32
	QueryPerformanceFrequency(&frequency);
	startCount.QuadPart = 0;
	endCount.QuadPart = 0;
#endif
	reset();
}

#ifdef WIN32
void StopWatch::reset() {
	QueryPerformanceCounter(&startCount);
        endCount = startCount;
}

void StopWatch::stop() {
	QueryPerformanceCounter(&endCount);
}

unsigned long long StopWatch::getUser() {
	unsigned long long start = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
	unsigned long long end = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
	return end - start;
}

unsigned long long StopWatch::getSystem() {
	return getUser();
}

unsigned long long StopWatch::getReal() {
	return getUser();
}

#else
void StopWatch::reset() {
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru);
	gettimeofday(&real1, NULL);

	memcpy(&user1, &ru.ru_utime, sizeof(struct timeval));
	memcpy(&system1, &ru.ru_stime, sizeof(struct timeval));

        memcpy(&user2, &user1, sizeof(struct timeval));
        memcpy(&system2, &system1, sizeof(struct timeval) );
}

void StopWatch::stop() {
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	gettimeofday(&real2, NULL);

	memcpy(&user2, &ru.ru_utime, sizeof(struct timeval));
	memcpy(&system2, &ru.ru_stime, sizeof(struct timeval));
}

unsigned long long difference(time_t s1, time_t s2, suseconds_t us1, suseconds_t us2) {
	unsigned long long tmp1, tmp2;

	tmp1 = s1 * 1000000 + us1;
	tmp2 = s2 * 1000000 + us2;

	return tmp1-tmp2;
}

unsigned long long StopWatch::getUser() {
	return difference(user2.tv_sec,user1.tv_sec, user2.tv_usec,user1.tv_usec);
}

unsigned long long StopWatch::getSystem() {
	return difference(system2.tv_sec,system1.tv_sec, system2.tv_usec,system1.tv_usec);
}

unsigned long long StopWatch::getReal() {
	return difference(real2.tv_sec,real1.tv_sec, real2.tv_usec,real1.tv_usec);
}
#endif





std::string StopWatch::getUserStr() {
	return toHuman(getUser());
}
std::string StopWatch::getSystemStr() {
	return toHuman(getSystem());
}
std::string StopWatch::getRealStr() {
	return toHuman(getReal());
}


unsigned long long StopWatch::stopUser() {
	stop();
	return getUser();
}

unsigned long long StopWatch::stopSystem() {
	stop();
	return getSystem();
}

unsigned long long StopWatch::stopReal() {
	stop();
	return getReal();
}


std::string StopWatch::stopUserStr() {
	stop();
	return getUserStr();
}

std::string StopWatch::stopSystemStr() {
	stop();
	return getSystemStr();
}

std::string StopWatch::stopRealStr() {
	stop();
	return getRealStr();
}

std::ostream &operator<<(std::ostream &stream, StopWatch &sw) {
	stream << sw.stopRealStr();
	return stream;
}

std::string StopWatch::toHuman(unsigned long long time) {
	unsigned int tot_secs = time/1000000;

	unsigned int hours = tot_secs/3600;
	unsigned int mins = (tot_secs/60) % 60;
	unsigned int secs = (tot_secs % 60);
	unsigned int ms = (time%1000000)/1000;
	unsigned int us = time%1000;

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
