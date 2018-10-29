/*
 * training.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mattias
 */

#include <iostream>
#include "network.h"
#include "valuemap.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;


vector<string> loadTrainingList(string filename) {
	vector<string> ret;

	std::ifstream input(filename);

	for(std::string line; getline( input, line );){
		ret.push_back(line);
	}

	return ret;
}

int main() {
	vector<string> filenames = loadTrainingList("trainingsets/list.txt");

	map<string, int> typeMap = {
			{"cats", 0},
			{"dogs", 1},
			{"houses", 2},
			{"cars", 3},
	};

	vector<TrainingSet> sets;

	for (auto name: filenames) {
		ValueMap input(loadValueMapImg("trainingsets/fixed/" + name));
//		input.normalze();
		input.prepare();
		size_t outputNum = 0;
		for (auto type: typeMap) {
			if(name.find(type.first) == 2) {
				outputNum = type.second;
				break;
			}
		}
		vector<ValueType> outputValues(typeMap.size());
		for (size_t i = 0; i < outputValues.size(); ++i) {
			outputValues[i] = (i == outputNum) ? 1 : 0;
		}
		ValueMap output(typeMap.size(), 1, 1, outputValues);

		sets.push_back({input, output});
	}

//	ValueMap inputMap(loadValueMap("linus.png", 100, 100));
//	ValueMap falseInputMap(loadValueMap("house.png", 100, 100));

//	inputMap.normalze();

//	ValueMap catOutputMap(2, 1, 1, {1, 0}); //A cat
//	TrainingSet set1(inputMap, catOutputMap);
//
//	ValueMap falseOutput(2, 1, 1, {0, 1}); //A house is not a cat
//	TrainingSet set2(falseInputMap, falseOutput);


//	InputLayer input(inputMap);
	Network network(new PatternTrainer(sets));


	auto layer1 = new ConvolutionLayer(network.back(), 5, 5);
	layer1->name = "conv layer 1";
	auto pool = new MaxPool(*layer1);
	auto layer2 = new ConvolutionLayer(*pool, 5, 5);
	layer2->name = "conv layer 2";
	auto pool2 = new MaxPool(*layer2);
	auto pool3 = new MaxPool(*pool2);
	auto std1 = new FullLayer(*pool3, 10);
	auto full2 = new FullLayer(*std1, typeMap.size());

	network.setChain(full2);

	size_t targetGeneration = 2000;
	for (size_t i = 0; i < targetGeneration; ++i) {
		network.backPropagationCycle();
		cout << "generation: " << i << "/" << targetGeneration << endl;
		cout << "Totalt fel: " << network.getTotalCost() << endl;
		full2->a.printXY();
		network.getCurrentTrainingSet().output.printXY();

		if (i % 20 == 0) {
			cout << "saving snap" << endl;
//			saveValueMap(full2->a, "output/output.png");
			network.trainer->getCurrent().output.printXY();

//			full2->save(cout);

			ofstream file("snapshot.txt");
			file << network;
		}
	}

//	showMap(inputMap);
//	showMap(layer1->net, "layer 1");
//	showMap(layer1->kernel, "layer 1 - kernel");
//	showMap(pool->a, "pool 1");
//	showMap(layer2->net, "layer 2");
//	showMap(layer2->kernel, "layer 2 - kernel");
//	showMap(pool2->a, "pool 2");
//	showMap(std1->a);
//	saveValueMap(full2->a, "output/output.png");
//	keepOpen();
	return 0;
}


