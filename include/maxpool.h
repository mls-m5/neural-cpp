/*
 * maxpool.h
 *
 *  Created on: 8 dec. 2018
 *      Author: Mattias Larsson Sköld
 */


#pragma once

#include "layer.h"

class MaxPool: public Layer {
public:
	struct Coords {int x, y;};
	std::vector<Coords> indexedCoords = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
	GenericValueMap<char> fromIndex;
	MaxPool(int width, int height, int depth):
		Layer(width, height, depth),
		fromIndex(width, height, depth){
		type = "maxpool";
	}

	//Disable standard copy constructor
	MaxPool(const MaxPool &) = delete;
	MaxPool(MaxPool & input):
		MaxPool((Layer&) input) {}

	MaxPool(std::istream &) {
		type = "maxpool";
		//
	}

	//Simpler initialization
	MaxPool(Layer& input):
	MaxPool(input.a.width() / 2, input.a.height() / 2, input.a.depth()){
		setSource(input);
		cout << "w " << input.a.width();
	}

	void forward() {
		auto &ain = source->a;
		mn_forXYZ(a, x, y, z) {
			auto tmpX = x * 2;
			auto tmpY = y * 2;

			a(x, y, z) = max4(
					ain(tmpX,     tmpY, z),
					ain(tmpX + 1, tmpY, z),
					ain(tmpX,     tmpY + 1, z),
					ain(tmpX + 1, tmpY + 1, z),
					fromIndex(x, y, z)
			);
		}
	}

	void prepareBackward() {
		d.fill(0);
	}

	void backward() {
		auto &sourceD = source->d;
		mn_forXYZ(d, x, y, z) {
			auto coord = indexedCoords[fromIndex(x, y, z)];
			sourceD(x + coord.x, y + coord.y, z) = d(x, y, z);
		}

	}

	static ValueType max4(ValueType a,ValueType b,ValueType c,ValueType d, char &from) {
		ValueType value = a;
		char index = 0;
		if (b > a) {
			value = b;
			index = 1;
		}
		if (c > b) {
			value = c;
			index = 2;
		}
		if (d > c) {
			value = d;
			index = 3;
		}
		from = index;
		return value;
	}


	//Call for hidden and output layers
	void correctErrors(ValueType ) override {
		//do nothing for max-pool
	}
};

