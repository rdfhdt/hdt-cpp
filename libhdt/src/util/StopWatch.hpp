/*
 * File: StopWatch.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias
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
 * Contacting the author:
 *   Mario Arias:               mario.arias@gmail.com
 *
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
