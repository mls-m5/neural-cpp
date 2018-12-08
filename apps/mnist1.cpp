/*
 * mnist1.cpp
 *
 *  Created on: 9 maj 2018
 *      Author: mattias
 */



#include <iostream>

#include "valuemap.h"
#include "network.h"
#include "alllayers.h"
#include "gui.h"
#include <fstream>

using namespace std;


int main() {
	cout << "Starting mnist test" << endl;

	auto minstData = loadMNISTBinary("../trainingsets/mnist");

	showMap(minstData.xtr[0], "test");


	vector<TrainingSet> sets;


	for (size_t i = 0; i < minstData.xtr.size(); ++i) {
		minstData.xtr[i].prepare();
		ValueMap output(10);
		output.fill(0);
		output[minstData.ytr[i]] = 1;
		sets.push_back({minstData.xtr[i], output});
	}

	Network network(new PatternTrainer(sets));
//	Network network(new LinearTrainer(sets));


	auto layer1 = new ConvolutionLayer(network.back(), 5, 5);
	layer1->name = "conv layer 1";
	auto pool = new MaxPool(*layer1);
	auto layer2 = new ConvolutionLayer(*pool, 5, 5);
	layer2->name = "conv layer 2";
	auto pool2 = new MaxPool(*layer2);
//	auto pool3 = new MaxPool(*pool2);
	auto std1 = new DenseLayer(*pool2, 10);
	auto full2 = new DenseLayer(*std1, 10);

	network.setChain(full2);

	size_t targetGeneration = 2000;
	for (size_t i = 0; i < targetGeneration; ++i) {
		network.backPropagationCycle(); //Todo: Det här krashar
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

	return 0;

	keepOpen();
}
