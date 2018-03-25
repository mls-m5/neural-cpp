/*
 * layer.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mattias
 */


#include "CImg.h"
#include "layer.h"
#include <fstream>
#include <iostream>

using namespace cimg_library;
using namespace std;

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

	for (size_t i; i < map.size(); ++i) {
		image[i] = map[i];
	}

	image.save(filename.c_str());
}



TrainingData loadCFAR10Binary(std::string pathname, size_t limit) {
	TrainingData data;

	size_t length = 60000;

	if (limit > 0) {
		length = limit;
	}
	const size_t batchLength = 10000;

	data.xtr.resize(length);
	data.ytr.resize(length);

	std::vector<std::string> filenames = {
		"data_batch_1.bin",
		"data_batch_2.bin",
		"data_batch_3.bin",
		"data_batch_4.bin",
		"data_batch_5.bin",
		"test_batch.bin",
	};

	// unsigned char charData[32 * 32 * 3];
	size_t currentData = 0;

	std::vector<unsigned char> rawData(batchLength * 6 * (32 * 32 * 3 + 1));
	const size_t fileLength = batchLength * (32 * 32 * 3 + 1);
	for (size_t i = 0; i < filenames.size(); ++i) {
		string fname = pathname + "/" + filenames[i];
		ifstream file(fname);

		if (!file.is_open()) {
			throw runtime_error("loadCFAR10Binary: could not read batch file");
		}

		file.read((char * ) &rawData[i * fileLength], fileLength);
	}

	size_t rawDataPos = 0;

	for (size_t i = 0; i < filenames.size(); ++i) {
		string fname = pathname + "/" + filenames[i];
		// ifstream file(fname);

		// if (!file.is_open()) {
		// 	throw runtime_error("loadCFAR10Binary: could not read batch file");
		// }

		for (size_t i = 0; i < batchLength && currentData < length; ++i, ++currentData) {
			// data.first[currentData] = file.get();
			data.ytr[currentData] = rawData[rawDataPos ++];
			// file.read((char*)charData, 32 * 32 * 3);
			ValueMap &map = data.xtr[currentData];
			map.resize(32, 32, 3, 1);
			int px = 0;
			unsigned int max = 0;
			unsigned int min = 256;
			long mean = 0;
			mn_forZ(map, z) 
			mn_forY(map, y) 
			mn_forX(map, x) 
			{
				// map(x,y,z) = file.get();
				// unsigned c = charData[px];
				unsigned c = rawData[rawDataPos++];
				map(x,y,z) = c;
				if (c < min) {
					min = c;
				}
				if (c > max) {
					max = c;
				}
				mean += c;

				// cout << (int) charData[px] << " ";
				++px;
			}

			float meanFloat = (float) mean / (32 * 32 * 3);

			map -= meanFloat;

			// do something with this

			// cout << endl;
			// map.normalze();
			// map.printXY();

			// cout << currentData << ": " << data.first[currentData] << endl;
		}
	}

	// Extract the test data to separate vectors
	data.xte = std::vector<ValueMap>(data.xtr.begin() + 50000, data.xtr.end());
	data.yte = std::vector<int>(data.ytr.begin() + 50000, data.ytr.end());

	data.xtr.resize(50000);
	data.xtr.shrink_to_fit();
	data.ytr.resize(50000);
	data.ytr.shrink_to_fit();

	return data;
}
