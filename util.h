/*
 * util.h
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */


#pragma once


#include <string>
#include <sstream>
#include <iostream>

double rnd();


inline std::string i2str(int i) {
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

