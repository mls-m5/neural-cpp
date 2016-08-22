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

struct DisplayData {
public:
	CImgDisplay *disp;
	ValueMap *map;
};

static std::vector<DisplayData> displays;

void waitDisplay(int ms) {
	displays.front().disp->wait(ms);
}

void showMap(ValueMap &map, std::string title) {
	CImg<float> image(map.width() * map.depth(), map.height() * map.spectrum());

	for (int c = 0; c < map.spectrum(); ++c) {
		for (int z = 0; z < map.depth(); ++z) {
			cimg_forXY(map, x, y) {
				image(x + map.width() * z, y + map.height() * c) = map(x, y, z, c);
			}
		}
	}

	std::stringstream ss;

	ss << title << " " << map.width() << "x" << map.height() << "x" << map.depth() << "x" << map.spectrum();

	if (image.width() < 100) {
		image.resize(100,100);
	}

	//Check if the map is alreay displayed, and update old display if so
	for (auto it: displays) {
		if (it.map == &map) {
			it.disp->assign(image);
			return;
		}
	}

	auto disp = new CImgDisplay(image, ss.str().c_str());
	displays.push_back({disp, &map});
}

void keepOpen() {
	auto &disp = *displays.front().disp;
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


