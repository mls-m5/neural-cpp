
/*
 * util.cpp
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */


#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1);


double rnd() {
	return dis(gen);
}
