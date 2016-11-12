

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "network.h"

void test1() {
	ValueMap inputMap(loadValueMap("linus.png", 100, 100));
	ValueMap falseInputMap(loadValueMap("house.png", 100, 100));

	inputMap.normalze();

	ValueMap catOutputMap(2, 1, 1, {1, 0}); //A cat
	TrainingSet set1(inputMap, catOutputMap);

	ValueMap falseOutput(2, 1, 1, {0, 1}); //A house is not a cat
	TrainingSet set2(falseInputMap, falseOutput);


//	InputLayer input(inputMap);
	Network network({set1, set2});


	auto layer1 = new ConvolutionLayer(network.back(), 5, 5);
	layer1->name = "conv layer 1";
	network.pushLayer(layer1);
	auto pool = new MaxPool(*layer1);
	network.pushLayer(pool);
	auto layer2 = new ConvolutionLayer(*pool, 5, 5);
	layer2->name = "conv layer 2";
	network.pushLayer(layer2);
	auto pool2 = new MaxPool(*layer2);
	network.pushLayer(pool2);
	auto pool3 = new MaxPool(*pool2);
	network.pushLayer(pool3);
	auto std1 = new FullLayer(*pool3, 10);
	network.pushLayer(std1);
	auto full2 = new FullLayer(*std1, 2);
	network.pushLayer(full2);

	size_t targetGeneration = 20;
	for (size_t i = 0; i < targetGeneration; ++i) {
		network.backPropagationCycle();
		cout << "generation: " << i << "/" << targetGeneration << endl;
		cout << "Totalt fel: " << network.getTotalCost() << endl;
	}

	showMap(inputMap);
	showMap(layer1->net, "layer 1");
	showMap(layer1->kernel, "layer 1 - kernel");
	showMap(pool->a, "pool 1");
	showMap(layer2->net, "layer 2");
	showMap(layer2->kernel, "layer 2 - kernel");
	showMap(pool2->a, "pool 2");
	showMap(std1->a);
	showMap(full2->a, "output");
	keepOpen();
}

void test2() {
	std::vector<std::vector<std::vector<ValueType>>> data = {
		{{0, 0}, {0, 1}},
		{{0, 1}, {1, 0}},
		{{1, 0}, {1, 1}},
		{{1, 1}, {0, 0}},
	};
	std::vector<TrainingSet> sets;

	for (auto it: data) {
		TrainingSet set;
		sets.push_back(TrainingSet(it[0], it[1]));
	}

	Network network(sets);
	auto hiddenLayer = new FullLayer(network.back(), 3);
	network.pushLayer(hiddenLayer);
	auto outLayer = new FullLayer(*hiddenLayer, 2);
	network.pushLayer(outLayer);


	for (int i = 0; i < 60000; ++i) {
		network.backPropagationCycle();


		if (i % 1001 == 0) {
			cout << "======= Generation: " << i << " ======================" << endl;
			mn_forX(outLayer->d, x) {
				cout << "out: " << outLayer->a[x] << " vs " << network.getCurrentTrainingSet().output(x) << " = training" << endl;
				cout << "outLayer.d: " << outLayer->d[x] << endl;
			}
			cout << "total error (C²): " << network.getTotalCost() << endl;
		}
	}

	showMap(network.layers[0]->a);
	showMap(outLayer->a, "output");
	showMap(outLayer->kernel, "out kernel");

//	waitDisplay(100);


	keepOpen();
}

void test3() { //Do a image filter and try to learn it
//	ValueMap inputMap(loadValueMap("linus.png", 100, 100));
//
//	InputLayer inputLayer(inputMap);
//	ConvolutionLayer targetConvolution(inputLayer, 1, 3);
//	targetConvolution.kernel.setData(std::vector<ValueType>({
//		0, -1, 0,
//		-1, 4, -1,
//		0, -1, 0
//	})); //Continue here...

}


int main(int, char **) {
	cout << "startar neuronnät" << endl;

	test1();
//	test2();


	std::exit(0);
}

