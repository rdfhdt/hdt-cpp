/*
 * fileUtil.cpp
 *
 *  Created on: 30/07/2011
 *      Author: mck
 */

#include "fileUtil.hpp"

using namespace std;

uint64_t fileUtil::getSize(std::istream &in) {
	long long begin = in.tellg();
	in.seekg(0, std::ios::end);
	long long end = in.tellg();
	in.seekg(begin, std::ios::beg);

	return end-begin;
}
