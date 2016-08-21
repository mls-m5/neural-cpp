

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "valuemap.h"


using std::cout;
using std::endl;

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

	static ValueType activationFunction(ValueType value) {
		return 1 / (1 + exp(-value));
	}

	static ValueType activationDerivate(ValueType value) {
		auto a = activationFunction(value);
	    return a * (1-a);
	}

};

class InputLayer: public Layer {
public:
	InputLayer(ValueMap &activations):
	Layer(activations.width() / 2, activations.height() / 2, 1)
	{
		a = activations;
	}

	virtual void forward() {};
	virtual void prepareBackward() {};
	virtual void backward() {};
};

class ConvolutionLayer: public Layer {
public:
//	valueMap net; //The summed input (z)
	ValueMap net;
	ValueMap kernel; //The connections

	ConvolutionLayer(int width, int height, int depth, int kernelSize, int kernelDepth):
	Layer(width, height, depth),
	net(width, height, depth),
	kernel(kernelSize, kernelSize, kernelDepth, depth){
		kernel.fill(1. / width / height);
//		kernel.noise(1, 1);
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
		auto &targetD = target->d;
		auto &targetA = target->a;
		auto &targetAPrim = target->aPrim;
		mn_forXYZ(targetD, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				//Kontrollera detta, jag är jättetrött när jag skriver det, så det kan mycket väl bli fel
				d(x + kx, y +ky, kz) += targetD(x, y, z) * targetAPrim(x, y, z) * kernel(kx, ky, kz);
			}
		}
	}

	void correctError() {

	}
};

class MaxPool: public Layer {
public:
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
					ain(tmpX, tmpY, z),
					ain(tmpX + 1, tmpY, z),
					ain(tmpX, tmpY, z),
					ain(tmpX + 1, tmpY + 1, z),
					fromIndex(x, y, z)
			);
		}
	}

	void prepareBackward() {}

	void backward() {

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
};


class StandardNetwork: public Layer {
public:
	ValueMap kernel; //w for the connections
	ValueMap net;

	StandardNetwork(Layer &layer, int number):
	Layer(number, 1, 1),
	kernel(layer.a.width(), layer.a.height(), layer.a.depth(), number),
	net(number, 1){
		setSource(layer);

		//Create random weights
		kernel.noise(1); //Add uniform noise
	}

	void forward() override {
		auto &input = source->a;
		mn_forX(net, ox) {
			mn_forXYZ(input, x, y, z) {
				net(ox) += input(x,y,z) * kernel(x, y, z, ox);
			}

			a(ox) = activationFunction(net(ox));
		}
	}

	void prepareBackward() override {}

	void backward() override {

	}
};


//Algorithm vanlig backpropagation
//1: Set input
//2: frammåtmatning
//3: Output error
//4: Backpropagate error
//5: Ändra vikter och bias

class TrainingSet {
public:
	ValueMap input;
	ValueMap output;
};


class Network {
public:
	std::vector<Layer*> layers;
	TrainingSet *currentTrainingSet;

	void pushLayer(Layer &layer) {
		layers.push_back(&layer);
	}

	void backPropagationCycle() {
		forwardPropagate();
		backPropagationCycle();
	}

	void forwardPropagate() {
		//set input...
		layers.front()->a = currentTrainingSet->input;

		//Forward propagate
		for (auto it = layers.begin() + 1; it != layers.end(); ++it) {
			(*it)->forward();
		}

	}

	void backPropagateError() {
		//Reset and prepare calculations
		for (auto layer: layers) {
			layer->prepareBackward();
		}

		//Calculate the output error
		layers.back()->d = layers.back()->a;
		layers.back()->d += currentTrainingSet->output;

		for (auto rit = layers.rbegin() + 1; rit != layers.rend(); ++rit) {
			(*rit)->backward();
		}
	}
};


int main() {
	cout << "startar neuronnät" << endl;

	ValueMap inputMap(loadValueMap("linus.png"));
	inputMap.normalze();

	InputLayer input(inputMap);


	ConvolutionLayer layer1(input, 2, 5);
	mn_forXYZC(layer1.kernel, x, y, z, c) {
		layer1.kernel(x, y, z) = 1;
	}
	layer1.forward();
	MaxPool pool(layer1);
	pool.forward();
	ConvolutionLayer layer2(pool, 2, 5);
	layer2.forward();
	MaxPool pool2(layer2);
	pool2.forward();
	MaxPool pool3(pool2);
	pool3.forward();
	StandardNetwork std1(pool3, 10);
	std1.forward();



	showMap(inputMap);
	showMap(layer1.net, "layer 1");
	showMap(layer1.kernel, "layer 1 - kernel");
	showMap(pool.a, "pool 1");
	showMap(layer2.net, "layer 2");
	showMap(pool2.a, "pool 2");
	keepOpen();

	std::exit(0);
}

