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
#include <functional>

#include <stdint.h> //For uint_32

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


// From https://stackoverflow.com/questions/12791864/c-program-to-check-little-vs-big-endian
//Check if system is big endian
int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}


const int systemBigEndian = is_big_endian();


int flipInt(int value) {
	union {
		int intIn;
		char charIn[4];
	};

	intIn = value;

	union {
		int intOut;
		char charOut[4];
	};

	for (size_t i = 0; i < sizeof(int); ++i) {
		charOut[3 - i] = charIn[i];
	}

	return intOut;
}

uint32_t correctMNISTEndian(uint32_t value) {
	if (systemBigEndian) {
		return value;
	}
	else {
		return flipInt(value);
	}
}

TrainingData loadMNISTBinary(std::string filename) {
	TrainingData data;

	auto loadImages = [] (std::string filename) {
		//File format image files
//		[offset] [type]          [value]          [description]
//		0000     32 bit integer  0x00000803(2051) magic number
//		0004     32 bit integer  60000            number of images
//		0008     32 bit integer  28               number of rows
//		0012     32 bit integer  28               number of columns
//		0016     unsigned byte   ??               pixel
//		0017     unsigned byte   ??               pixel
//		........
//		xxxx     unsigned byte   ??               pixel

		ifstream file(filename);
		uint32_t rawData[4];

		file.read((char*)rawData, sizeof (uint32_t) * 4);

//		uint32_t magicNumber = correctMNISTEndian(rawData[0]);
		uint32_t imageCount = correctMNISTEndian(rawData[1]);
		uint32_t rows = correctMNISTEndian(rawData[2]);;
		uint32_t columns = correctMNISTEndian(rawData[3]);;

//		cout << "magick number " << magicNumber << endl;
//		cout << "number of images " << imageCount << endl;
//		cout << "dimensions: " << rows << "x" << columns << endl;

//		file.read((char * ) &rawData[i * fileLength], fileLength);

		ValueMapVector maps;
		maps.resize(imageCount);

		vector<unsigned char> rawPixels(rows * columns);

		for (auto &map: maps) {
			map.resize(rows, columns);
			file.read((char *) &rawPixels[0], rows * columns);
			map.setData(rawPixels);
		}

		return maps;
	};

	auto loadLabels = [] (string filename) {
//	[offset] [type]          [value]          [description]
//	0000     32 bit integer  0x00000801(2049) magic number (MSB first)
//	0004     32 bit integer  60000            number of items
//	0008     unsigned byte   ??               label
//	0009     unsigned byte   ??               label
//	........
//	xxxx     unsigned byte   ??               label
//	The labels values are 0 to 9.


		ifstream file(filename);
		uint32_t rawData[4];

		file.read((char*)rawData, sizeof (uint32_t) * 2);

//		uint32_t magicNumber = correctMNISTEndian(rawData[0]);
		uint32_t imageCount = correctMNISTEndian(rawData[1]);

//		cout << "magick number " << magicNumber << endl;
//		cout << "number of images " << imageCount << endl;

		vector<unsigned char> rawLabels(imageCount);

		file.read((char *) &rawLabels[0], imageCount);

		LabelVector labels(rawLabels.begin(), rawLabels.end());

		return labels;
	};

	data.xtr = loadImages(filename + "/" + "train-images.idx3-ubyte");
	data.ytr = loadLabels(filename + "/" + "train-labels.idx1-ubyte");

	data.xte = loadImages(filename + "/" + "t10k-images.idx3-ubyte");
	data.yte = loadLabels(filename + "/" + "t10k-labels.idx1-ubyte");

	return data;
}


