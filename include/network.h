/*
 * network.h
 *
 *  Created on: 22 aug 2016
 *      Author: mattias
 */

#pragma once

#include "valuemap.h"
#include "layer.h"
#include "trainingset.h"
#include "trainer.h"

#include <exception>
#include <memory>
#include <map>

//Algorithm common backpropagation
//1: Set input
//2: frammåtmatning
//3: Output error
//4: Backpropagate error
//5: Ändra vikter och bias



class Network {
public:
	std::vector<Layer*> layers;
//	size_t currentTrainingSet = 0;
//	std::vector<TrainingSet> trainingSets;
	ValueType learningRate = .1;
	std::unique_ptr<Trainer> trainer;

	Network(std::vector<TrainingSet> trainingSets):
		trainer(new LinearTrainer(trainingSets)){
//	trainingSets(trainingSets){
//		currentTrainingSet = 0;
		auto &set = this->getCurrentTrainingSet();
		ValueMap inMap(set.input.width(), set.input.height(), set.input.depth());
		layers.push_back(new InputLayer(inMap));
	}

	Network(Trainer *trainer, int w = 0, int h = 0, int d = 0):
		trainer(trainer) {
		if (w == 0 && h == 0 && d == 0) {
			layers.push_back(new InputLayer(w, h, d));
		}
		else {
			auto &set = this->trainer->getCurrent();
			ValueMap inMap(set.input.width(), set.input.height(), set.input.depth());
			layers.push_back(new InputLayer(inMap));
		}
	}

	~Network() {
		for (auto it: layers) {
			delete it;
		}
	}

	Layer &inputLayer() {
		return *layers[0];
	}

	Layer &back() {
		return *layers.back();
	}

	void pushLayer(Layer *layer) {
		layers.push_back(layer);
	}
	
	//Add a whole chain of layers by only adding the last layer in the chain
	void setChain(Layer *layer) {
		auto beginningSize = layers.size();
		for (auto l = layer; l->source != nullptr; l = l->source) {
			layers.insert(layers.begin() + beginningSize, l);
		}
		if (beginningSize > 0) {
			layers[beginningSize]->source = layers[beginningSize - 1];
		}
	}

	void save(std::ostream &stream) {
		for (auto &l: layers) {
			stream << *l;
		}
	}

	void load(std::istream &stream) {
		auto previous = layers.front();
		while (auto l = Layer::load(stream, *previous)) {
			layers.push_back(l);
		}
	}

	void backPropagationCycle() {
		rotateTrainingsets();
		layers.front()->a = getCurrentTrainingSet().input;
		forwardPropagate();
		backPropagateError();
		correctErrors();
	}

	void dreamCycle() {
		inputLayer().aPrim.fill(1);
		forwardPropagate();
		backPropagateError();
		correctInputLayer();
	}

	TrainingSet &getCurrentTrainingSet() {
//		return trainingSets[currentTrainingSet];
		return trainer->getCurrent();
	}

	void forwardPropagate() {

		//Forward propagate
		for (auto it = layers.begin() + 1; it != layers.end(); ++it) {
			(*it)->forward();
		}
	}

	ValueType getTotalCost() {
		auto &outputAData = back().a;
		auto &outputTraining = getCurrentTrainingSet().output;
		if (!outputAData.isDimensionsSame(outputTraining)) {
			throw std::range_error(std::string(__FILE__) + ":" + util::i2str(__LINE__) + ": " + __func__ + "(): training data " + outputTraining.toString() +
					" does not match network layer " + outputAData.toString() + " dimensions");
		}
		ValueType sum = 0;
		mn_forXYZ(outputAData, x, y, z) {
			auto diff = outputAData(x, y, z) - outputTraining(x, y, z);
			sum += diff * diff;
		}
		return sum;
	}

	void rotateTrainingsets() {
		trainer->rotate();
//		if (++currentTrainingSet >= trainingSets.size()) {
//			currentTrainingSet = 0;
//		}
	}

	void backPropagateError() {

		//Reset and prepare calculations
		for (auto &layer: layers) {
			layer->prepareBackward();
		}

		//Calculate the output error
		auto &outputLayer = back();
		outputLayer.d = outputLayer.a;
		outputLayer.d -= getCurrentTrainingSet().output;
		outputLayer.d *= outputLayer.aPrim;

		for (auto rit = layers.rbegin(); rit != layers.rend(); ++rit) {
			//Effect on input layer is only used when dreaming
			(*rit)->backward();
		}
	}

	//The last step in the back propagation cycle
	void correctErrors() {
		if (layers.empty()) {
			return;
		}
		for (auto it=layers.begin() + 1; it!= layers.end(); ++it){
			(*it)->correctErrors(learningRate);
		}
	}

	//Used for dreaming
	//Correct the error on the input layer without changing anything else
	void correctInputLayer() {
		if (layers.empty()) {
			return;
		}
		layers.front()->correctErrors(learningRate);
	}
};


inline std::ostream &operator<<(std::ostream &stream, Network &network) {
	network.save(stream);

	return stream;
}


inline std::istream &operator>>(std::istream &stream, Network &network) {
	network.load(stream);

	return stream;
}


