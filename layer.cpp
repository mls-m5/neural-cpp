/*
 * layer.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mattias
 */


#include "CImg.h"
#include "layer.h"

using namespace cimg_library;

ValueMap loadValueMapImg(std::string filename, int nx, int ny) {
	CImg<float> image(filename.c_str());

	if (nx > 0 || ny > 0) {
		if (nx <= 0) {
			nx = image.width();
		}
		if (ny <= 0) {
			ny = image.height();
		}
		image.resize(nx, ny);
	}

	ValueMap map(image.width(), image.height(), image.spectrum());
	cimg_forXYC(image, x, y, c) {
		map(x, y, c) = image(x, y, 0, c); //Move the color component to z
	}

	return map;
}

void saveValueMapImg(ValueMap &map, std::string filename) {
	CImg<ValueType> image(map.width(), map.height(), map.depth(), map.spectrum());

	for (size_t i; i < map.data().size(); ++i) {
		image[i] = map[i];
	}

	image.save(filename.c_str());
}

