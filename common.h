/*
 * common.h
 *
 *  Created on: 12 nov. 2016
 *      Author: mattias
 */

#pragma once

#include <string>
#include <sstream>

inline std::string i2str(int i) {
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

