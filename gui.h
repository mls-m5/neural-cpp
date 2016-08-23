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
void showMap(ValueMap &map, std::string title = "map");
ValueMap loadValueMap(std::string filename, size_t newWidth = -1, size_t newHeight = -1);
void waitDisplay(int ms);


