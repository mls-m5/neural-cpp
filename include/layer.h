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
	enum ActivationFunction {
		Relu,
		Sigmoid,
		Softmax,
	};

	Layer *source = 0;
	Layer *target = 0;
	ValueMap a;
	ValueMap aPrim;
	ValueMap d;
	ActivationFunction _activationFunctionType = Sigmoid;

	std::string name = ""; //Used to identify the layer
	std::string type = "none"; //Name that is used for saving

	Layer(int width, int height, int depth = 1, ActivationFunction f = Sigmoid):
	a(width, height, depth),
	aPrim(width, height, depth),
	d(width, height, depth),
	_activationFunctionType(f){
	}

	Layer() = default;

	void resize(int width, int height = 1, int depth = 1, int spectrum = 1) {
		a.resize(width, height, depth, spectrum);
		aPrim.resize(width, height, depth, spectrum);
		d.resize(width, height, depth, spectrum);
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
	virtual void saveHeader(std::ostream &stream) {
		stream << type << " '" << name << "'" << std::endl;
	}
	virtual void save(std::ostream &stream) {
		saveHeader(stream);
	}

	static Layer* load(std::istream& stream, Layer &from);

	//Call for hidden and output layers
	virtual void correctErrors(ValueType learningRate) = 0;

	ActivationFunction activationFunctionType() {
		return _activationFunctionType;
	}

	void activationFunctionType(ActivationFunction f) {
		_activationFunctionType = f;
	}

	//calculate the activation fuction on a whole value map
	static void activationFunction(ActivationFunction type, const ValueMap &input, ValueMap &output) {
		switch (type) {
		case Sigmoid:
			mn_for1(input.size(), i) {
				output[i] = sigmoidFunction(input[i]);
			}
			break;
		case Relu:
			mn_for1(input.size(), i) {
				output[i] = ReluFunction(input[i]);
			}
			break;
		case Softmax:
			//Todo: implement this
			break;
		}
	}

	static void activationFunction(
			ActivationFunction type, const ValueMap &input, ValueMap &output, ValueMap &derivate) {
		activationFunction(type, input, output);
		activationDerivate(type, input, output, derivate); //This can be optimized for example for sigmoid functions
	}

	static void activationDerivate(
			ActivationFunction type, const ValueMap &input, const ValueMap &computedValue, ValueMap &derivativeOut) {
		switch (type) {
		case Sigmoid:
			mn_for1(input.size(), i) {
				derivativeOut[i] = sigmoidDerivate(computedValue[i]);
			}
			break;
		case Relu:
			mn_for1(input.size(), i) {
				derivativeOut[i] = ReluDerivative(input[i]);
			}
			break;
		case Softmax:

			break;
		}
	}

	static inline ValueType ReluFunction(ValueType in) {
		return in * (in > 0);
	}

	static inline ValueType ReluDerivative(ValueType in) {
		return in > 0;
	}

	static inline ValueType sigmoidFunction(ValueType value) {
		return 1. / (1. + exp(-value));
	}

	//Note that this function as input takes the output of a sigmoidFunction
	static inline ValueType sigmoidDerivate(ValueType computedValue) {
//		auto a = sigmoidFunction(value); //Like this
//	    return a * (1.-a);
		return computedValue * (1. - computedValue);
	}
};


inline std::ostream &operator<<(std::ostream &stream, Layer &layer) {
	layer.save(stream);

	return stream;
}


class InputLayer: public Layer {
public:
	InputLayer(ValueMap &activations):
	Layer(activations.width(), activations.height(), activations.depth())
	{
		type = "input";
		a = activations;
	}


	InputLayer(dim_t w, dim_t h, dim_t d):
		Layer(w, h, d)
	{
		type = "input";
	}

	//input layers does not do anything
	virtual void forward() override {};
	virtual void prepareBackward() override {};
	virtual void backward() override {};
	virtual void correctErrors(ValueType learningRate) override {
		mn_forXYZ(a, x, y, z) {
			a(x, y, z) -= d(x, y, z) * learningRate;
		}
	}
};


