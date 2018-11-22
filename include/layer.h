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
#pragma warning("check if this really should correct errors in a")
		mn_forXYZ(a, x, y, z) {
			a(x, y, z) -= d(x, y, z) * learningRate;
		}
	}
};

class ConvolutionLayer: public Layer {
public:
	ValueMap net; //The summed input (z)
	ValueMap kernel; //The weight of the connections, shared
	ValueMap bias; //The bias of the connections, also shared

	ConvolutionLayer(int width, int height, int depth, int kernelSize, int kernelDepth):
	Layer(width, height, depth),
	net(width, height, depth),
	kernel(kernelSize, kernelSize, kernelDepth, depth),
	bias(kernelSize, kernelSize, kernelDepth, depth){
		type = "conv";
		kernel.noise(1);
		bias.noise(1);
	}

	ConvolutionLayer(const ConvolutionLayer&) = delete;

	ConvolutionLayer(std::istream &stream, Layer &input):
		kernel(stream),
		bias(stream) {
		type = "conv";
		auto &ia = input.a;
		resize(ia.width(), ia.height(), ia.depth());
	}

	ConvolutionLayer(Layer &input, int depth, int kernelSize):
	ConvolutionLayer(input.a.width() - kernelSize + 1,
	input.a.height() - kernelSize + 1, depth, kernelSize, input.a.depth()){
		setSource(input);
	}

	void save(std::ostream &stream) override {
		saveHeader(stream);
		kernel.save(stream);
		bias.save(stream);
	}

	void forward() override {
		net.fill(0);
		//Not that the kz does not work in the same convolving way as the others
		//All layers in the output layer should be connected to all the neuron in
		//z-axis but not in the other axes
		//Also note that  the kernel has four dimensions 3 for the output layer and 1 (c) for the input layer
		//That is because the kernel is shared for all the neurons in the output layers xy axis (0 dimensions)
		//but not for the z axis (1 dimension)
		mn_forXYZ(net, x, y, z) {
			ValueType n = 0;
			mn_forXYZ(kernel, kx, ky, kz) {
				auto w = kernel(kx, ky, kz, z); //Note that the last dimension is the separated z axis
				n += source->a(x + kx, y + ky, kz) * w + bias(kx, ky, kz, z);
			}
			net (x, y, z) = n;
		}

//		mn_forXYZ(a, x, y, z) {
//			auto n = net(x, y, z);
//			a(x, y, z) = sigmoidFunction(n); // Add activation function
//			aPrim(x, y, z) = sigmoidDerivate(n); //Calculate derivative, this can be optimized
//		}

		activationFunction(activationFunctionType(), net, a, aPrim);
	}

	void prepareBackward() override {
		d.fill(0);
	}

	void backward() override {
		auto &sourceD = source->d;
		mn_forXYZ(d, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				//Like the forward propagation above but backwards (and the derivative so there is no bias)
				sourceD(x + kx, y + ky, kz) += d(x, y, z) * aPrim(x, y, z) * kernel(kx, ky, kz, z);
			}
		}
	}

	//Call for hidden and output layers
	void correctErrors(ValueType learningRate) override {
		auto &sourceA = source->a; //I have no idea, check so that it works
//		mn_forXYZ(bias, x, y, z) {
//			bias(x, y, z) -= d(x, y, z) * learningRate;
//		}

		double scale = 1. / kernel.data().size();
		mn_forXYZ(d, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				bias(kx, ky, kz, z) -= d(x, y, z) * learningRate * scale;
				kernel(kx, ky, kz, z) -= d(x, y, z) * sourceA(x, y, kz) * learningRate * scale;
			}
		}

		//Prototype from fully connected layer
//		auto &sourceA = source->a;
//		mn_forX(bias, x) {
//			bias(x) -= d(x) * learningRate;
//		}
//		mn_forX(net, ox) {
//			mn_forXYZ(kernel, x, y, z) {
//				kernel(x, y, z, ox) -= this->d(ox) * sourceA(x, y, z) * learningRate;
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


class FullLayer: public Layer {
public:
	int number = 1;
	ValueMap kernel; //w for the connections
	ValueMap bias;
	ValueMap net;

	FullLayer(Layer &layer, int number, ActivationFunction activationFunctionType = Sigmoid):
	Layer(number, 1, 1, activationFunctionType),
	number(number),
	kernel(layer.a.width(), layer.a.height(), layer.a.depth(), number),
	bias(number, 1),
	net(number, 1){
		setSource(layer);

		//Create random weights
		kernel.noise(1); //Add uniform noise
		kernel.noise(1);

		type = "full";
	}

	FullLayer(std::istream &stream, Layer &layer)
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

inline Layer* Layer::load(std::istream& stream, Layer &from) {
	std::string type, name;
	stream >> type >> name;
	
	if (type == "conv") {
		return new ConvolutionLayer(stream, from);
	}
	if (type == "maxpool") {
		return new MaxPool(from); //Note the max-pool does not load any data
	}
	if (type == "full") {
		return new FullLayer(stream, from);
	}
	else {
		return nullptr;
	}
}
