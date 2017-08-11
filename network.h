/*
 * network.h
 *
 *  Created on: 22 aug 2016
 *      Author: mattias
 */

#pragma once

#include "valuemap.h"
#include "layer.h"

#include <exception>
#include <memory>
#include <map>

//Algorithm common backpropagation
//1: Set input
//2: frammåtmatning
//3: Output error
//4: Backpropagate error
//5: Ändra vikter och bias

class TrainingSet {
public:
	template<class T>
	TrainingSet(std::vector<T> in, std::vector<T> out):
	input(in.size(), 1),
	output(out.size(), 1){
		for (size_t i = 0; i < in.size(); ++i) {
			input[i] = in[i];
		}
		for (size_t i = 0; i < out.size(); ++i) {
			output[i] = out[i];
		}
    };
	template<class T>
	TrainingSet(std::vector<std::vector<T>> set):
	TrainingSet(set[0], set[1])
	{}

	TrainingSet(const TrainingSet& set) {
		input = set.input;
		output = set.output;
	}
	TrainingSet() = default;

	TrainingSet(const ValueMap &in, const ValueMap &out):
	input(in), output(out){
	}
	ValueMap input;
	ValueMap output;
};


class Trainer {
public:
	virtual ~Trainer() {}

	virtual void rotate() = 0;

	virtual TrainingSet &getCurrent() = 0;
};

class LinearTrainer: public Trainer {
public:
	std::vector<TrainingSet> _sets;
	size_t _current = 0;

	LinearTrainer(std::vector<TrainingSet> trainingSets) {
		_sets = trainingSets;
	}

	LinearTrainer() = default;
	LinearTrainer(const LinearTrainer &) = default;
	LinearTrainer(LinearTrainer &&) = default;

	void rotate() override {
		if (++_current >= _sets.size()) {
			_current = 0;
		}
	}

	TrainingSet &getCurrent() override {
		return _sets[_current];
	}

	void add(TrainingSet set) {
		_sets.push_back(std::move(set));
	}
};

//Order the training sets after which output that is the strongest
//Good for pattern recognition training
class PatternTrainer: public Trainer {
protected:
	std::map<int, LinearTrainer> _subTrainers;
	size_t _current = 0;
public:


	PatternTrainer(std::vector<TrainingSet> trainingSets) {
		for (auto &it: trainingSets) {
			add(it);
		}
	}

	PatternTrainer() = default;
	PatternTrainer(const PatternTrainer &) = default;
	PatternTrainer(PatternTrainer &&) = default;

	void rotate() override {
		if (++_current >= _subTrainers.size()) {
			_current = 0;
		}
		_subTrainers[_current].rotate();
	}

	TrainingSet &getCurrent() override {
		return _subTrainers[_current].getCurrent();
	}


	void add(TrainingSet set) {
		if (set.output.height() != 1 || set.output.depth() != 1) {
			throw std::range_error("Trainingset height and depth must be 1 for PatternTrainer");
		}

		ValueType max = -10000000;
		int maxNumber = -1;
		for (auto i = 0; i < set.output.width(); ++i) {
			if (set.output(i) > max) {
				max = set.output(i);
				maxNumber = i;
			}
		}
		_subTrainers[maxNumber].add(set);
	}

};


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
	Network(Trainer *trainer):
		trainer(trainer) {
		auto &set = this->trainer->getCurrent();
		ValueMap inMap(set.input.width(), set.input.height(), set.input.depth());
		layers.push_back(new InputLayer(inMap));
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
			throw std::range_error(std::string(__FILE__) + ":" + i2str(__LINE__) + ": " + __func__ + "(): training data " + outputTraining.toString() +
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


