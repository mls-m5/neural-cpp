/*
 * gui.h
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */



#pragma once

#include "valuemap.h"
#include <string>

void keepOpen();
void showMap(ValueMap &map, std::string title = "map", bool separateChannels = false);
void waitDisplay(int ms);


