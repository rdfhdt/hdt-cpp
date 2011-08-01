/*
 * fileUtil.hpp
 *
 *  Created on: 30/07/2011
 *      Author: mck
 */

#ifndef FILEUTIL_HPP_
#define FILEUTIL_HPP_

#include <stdint.h>
#include <iostream>

class fileUtil {
public:
	static uint64_t getSize(std::istream &in);
};

#endif /* FILEUTIL_HPP_ */
