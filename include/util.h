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
#include <random>

namespace util {

double rnd();


std::uniform_real_distribution <> &unity_distribution();
std::mt19937 &random_engine();

inline std::string i2str(int i) {
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

}
