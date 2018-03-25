
/*
 * util.cpp
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */


#include <random>
#include <functional>
#include "util.h"

namespace util {

static std::random_device rd;
static std::mt19937 eng(rd());
static std::uniform_real_distribution<> dis(0, 1);


double rnd() {
	return dis(eng);
}

std::uniform_real_distribution <> &unity_distribution() {
    return dis;
}

std::mt19937 &random_engine() {
    return eng;
}

}

