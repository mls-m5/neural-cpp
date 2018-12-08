/*
 * denselayer.h
 *
 *  Created on: 8 dec. 2018
 *      Author: Mattias Larsson Sköld
 */


#pragma once

#include "layer.h"

class DenseLayer: public Layer {
public:
	int number = 1;
	ValueMap kernel; //w for the connections
	ValueMap bias;
	ValueMap net;

	DenseLayer(Layer &layer, int number, ActivationFunction activationFunctionType = Sigmoid):
	Layer(number, 1, 1, activationFunctionType),
	number(number),
	kernel(layer.a.width(), layer.a.height(), layer.a.depth(), number),
	bias(number, 1),
	net(number, 1){
		setSource(layer);

		//Create random weights
		kernel.noise(1); //Add uniform noise
		kernel.noise(1);

		type = "dense";
	}

	DenseLayer(std::istream &stream, Layer &layer)
	{
		stream >> number;
		resize(layer.a.width(), layer.a.height(), layer.a.depth(), number);
		kernel.load(stream);
		bias.load(stream);
	}

	void save(std::ostream &stream) override {
		saveHeader(stream);
		stream << number << endl;
		kernel.save(stream);
		bias.save(stream);
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
//			a(ox) = sigmoidFunction(n);
//			aPrim(ox) = sigmoidDerivate(n);
		}

		activationFunction(activationFunctionType(), net, a, aPrim);
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

