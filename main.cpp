

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "network.h"

void test1() {
	ValueMap inputMap(loadValueMap("linus.png", 100, 100));
	inputMap.normalze();

	ValueMap outputMap(2, 1, 1, {1, 0}); //A cat an not a dog
	TrainingSet set(inputMap, outputMap);


//	InputLayer input(inputMap);
	Network network({set});



	auto layer1 = new ConvolutionLayer(network.back(), 2, 5);
	network.pushLayer(layer1);
//	mn_forXYZC(layer1.kernel, x, y, z, c) {
//		layer1.kernel(x, y, z, c) = rnd() + c;
//	}
//	layer1.forward();
	auto pool = new MaxPool(*layer1);
//	pool.forward();
	network.pushLayer(pool);
	auto layer2 = new ConvolutionLayer(*pool, 2, 5);
	network.pushLayer(layer2);
//	layer2.forward();
	auto pool2 = new MaxPool(*layer2);
	network.pushLayer(pool2);
//	pool2.forward();
	auto pool3 = new MaxPool(*pool2);
	network.pushLayer(pool3);
//	pool3.forward();
	auto std1 = new FullLayer(*pool3, 10);
	network.pushLayer(std1);
//	std1.forward();
	auto full2 = new FullLayer(*std1, 2);
	network.pushLayer(full2);
//	full2.forward();


//	network.forwardPropagate();
	for (int i = 0; i < 100; ++i) {
		network.backPropagationCycle();

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


int main(int, char **) {
	cout << "startar neuronnät" << endl;

	test1();
//	test2();


	std::exit(0);
}

