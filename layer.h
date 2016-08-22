/*
 * layers.h
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */

#pragma once

#include "valuemap.h"
#include <math.h>


using std::cout;
using std::endl;

using ::exp;

class Layer {
public:
	Layer *source = 0;
	Layer *target = 0;
	ValueMap a;
	ValueMap aPrim;
	ValueMap d;

	Layer(int width, int height, int depth = 1):
	a(width, height, depth),
	aPrim(width, height, depth),
	d(width, height, depth){
	}

	virtual ~Layer() {

	}

	virtual void setSource(Layer& source) {
		this->source = &source;
		source.target = this;
	}
	virtual void forward() = 0;
	virtual void prepareBackward() = 0;
	virtual void backward() = 0;

	//Call for hidden and output layers
	virtual void correctErrors(ValueType learningRate) = 0;

	static ValueType activationFunction(ValueType value) {
		return 1. / (1. + exp(-value));
	}

	static ValueType activationDerivate(ValueType value) {
		auto a = activationFunction(value);
	    return a * (1.-a);
	}

};

class InputLayer: public Layer {
public:
	InputLayer(ValueMap &activations):
	Layer(activations.width(), activations.height())
	{
		a = activations;
	}

	//input layers does not do anything
	virtual void forward() override {};
	virtual void prepareBackward() override {};
	virtual void backward() override {};
	virtual void correctErrors(ValueType) override {}
};

class ConvolutionLayer: public Layer {
public:
	ValueMap net; //The summed input (z)
	ValueMap kernel; //The connections

	ConvolutionLayer(int width, int height, int depth, int kernelSize, int kernelDepth):
	Layer(width, height, depth),
	net(width, height, depth),
	kernel(kernelSize, kernelSize, kernelDepth, depth){
		kernel.noise(1);
		mn_forX(kernel, x) {
			cout << "kernel" << kernel(x, 0,0) << endl;
		}
	}

	ConvolutionLayer(const ConvolutionLayer&) = delete;

	ConvolutionLayer(Layer &input, int depth, int kernelSize):
	ConvolutionLayer(input.a.width() - (kernelSize - 1) / 2,
	input.a.height() - (kernelSize - 1) / 2, depth, kernelSize, input.a.depth()){
		setSource(input);
	}

	void forward() override {
		net.fill(0);
		//Not that the kz does not work in the same convolving way as the others
		//All layers in the output layer should be connected to all the neuron in
		//z-axis but not in the other axes
		mn_forXYZ(kernel, kx, ky, kz) {
			mn_forXYZ(net, x, y, z) {
				auto k = kernel(kx, ky, kz, z); //For debugging
				net(x, y, z) += source->a(x + kx, y +ky, kz) * k;
			}
		}

		mn_forXYZ(a, x, y, z) {
			auto n = net(x, y, z);
			a(x, y, z) = activationFunction(n); // Add activation function here
			aPrim(x, y, z) = activationDerivate(n); //Calculate derivative, this can be optimized
		}
	}

	void prepareBackward() override {
		d.fill(0);
	}

	void backward() override {
		auto &sourceD = source->d;
		mn_forXYZ(d, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				//Kontrollera detta, jag är jättetrött när jag skriver det, så det kan mycket väl bli fel
				sourceD(x + kx, y +ky, kz) += d(x, y, z) * aPrim(x, y, z) * kernel(kx, ky, kz);
			}
		}
	}

	//Call for hidden and output layers
	void correctErrors(ValueType learningRate) override {
//		auto &sourceA = source->a; //I have no idea, check so that it works
//		mn_forX(bias, x) {
//			bias(x) -= d(x) * learningRate;
//		}
//		mn_forXYZ(net, x, y, z) {
//			mn_forXYZ(kernel, kx, ky, kz) {
//				kernel(kx, ky, kz, ) -= this->d(ox) * sourceA(x, y, z) * learningRate;
//			}
//		}
	}


};


class MaxPool: public Layer {
public:
	struct Coords {int x, y;};
	std::vector<Coords> indexedCoords = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
	GenericValueMap<char> fromIndex;
	MaxPool(int width, int height, int depth):
		Layer(width, height, depth),
		fromIndex(width, height, depth){
	}

	//Disable standard copy constructor
	MaxPool(const MaxPool &) = delete;
	MaxPool(MaxPool & input):
		MaxPool((Layer&) input) {}

	//Simpler initialization
	MaxPool(Layer& input):
	MaxPool(.5 * input.a.width(), .5 * input.a.height(), input.a.depth()){
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
			auto coord = indexedCoords(fromIndex);
			sourceD(x + coord, y + coord, z) = d(x, y, z);
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


class FullLayer: public Layer {
public:
	ValueMap kernel; //w for the connections
	ValueMap bias;
	ValueMap net;

	FullLayer(Layer &layer, int number):
	Layer(number, 1, 1),
	kernel(layer.a.width(), layer.a.height(), layer.a.depth(), number),
	bias(number, 1),
	net(number, 1){
		setSource(layer);

		//Create random weights
		kernel.noise(1); //Add uniform noise
		kernel.noise(1);
	}

	void forward() override {
		auto &input = source->a;
		mn_forX(net, ox) {
			ValueType n = 0;
			//Sum the input from the other neurons
			mn_forXYZ(input, x, y, z) {
				n += input(x,y,z) * kernel(x, y, z, ox) + bias(ox);
			}

			net(ox) = n;
			a(ox) = activationFunction(n);
			aPrim(ox) = activationDerivate(n);
		}
	}

	void prepareBackward() override {
		d.fill(0);
	}

	void backward() override {
		auto &sourceD = source->d;
		auto &sourceAPrim = source->aPrim;
		mn_forX(net, ox) {
			mn_forXYZ(sourceD, x, y, z) {
				sourceD(x, y, z) += kernel(x, y, z, ox) * d(ox) * sourceAPrim(x, y, z);
			}
		}
	}

	void correctErrors(ValueType learningRate) {
		auto &sourceA = source->a;
		mn_forX(bias, x) {
			bias(x) -= d(x) * learningRate;
		}
		mn_forX(net, ox) {
			mn_forXYZ(kernel, x, y, z) {
				kernel(x, y, z, ox) -= this->d(ox) * sourceA(x, y, z) * learningRate;
			}
		}
	}
};

