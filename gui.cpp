/*
 * gui.cpp
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */


#include "gui.h"
#include "CImg.h"
#include <sstream>

using namespace cimg_library;

std::vector<CImgDisplay *> displays;

void showMap(ValueMap &map, std::string title) {
	CImg<float> image(map.width() * map.depth(), map.height());
	for (int c = 0; c < map.depth(); ++c) {
		cimg_forXY(map, x, y) {
			image(x + map.width() * c, y) = map(x, y, c);
		}
	}

	std::stringstream ss;

	ss << title << " " << map.width() << "x" << map.height() << "x" << map.depth();

	auto disp = new CImgDisplay(image, ss.str().c_str());
	displays.push_back(disp);
}

void keepOpen() {
	auto &disp = *displays.front();
	while (!disp.is_closed() && !disp.is_keyQ() && !disp.is_keyESC()) {
		disp.wait_all();
	}
}

ValueMap loadValueMap(std::string filename) {
	CImg<float> image(filename.c_str());
	ValueMap map(image.width(), image.height());
	cimg_forXY(image, x, y) {
		cimg_forC(image, c) {
			map(x, y) += image(x, y, 0, c);
		}
	}

	return map;
}


