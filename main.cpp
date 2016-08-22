

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "network.h"

void test1() {
	ValueMap inputMap(loadValueMap("linus.png"));
	inputMap.normalze();

	InputLayer input(inputMap);


	ConvolutionLayer layer1(input, 2, 5);
//	mn_forXYZC(layer1.kernel, x, y, z, c) {
//		layer1.kernel(x, y, z, c) = rnd() + c;
//	}
	layer1.forward();
	MaxPool pool(layer1);
	pool.forward();
	ConvolutionLayer layer2(pool, 2, 5);
	layer2.forward();
	MaxPool pool2(layer2);
	pool2.forward();
	MaxPool pool3(pool2);
	pool3.forward();
	FullLayer std1(pool3, 10);
	std1.forward();



	showMap(inputMap);
	showMap(layer1.net, "layer 1");
	showMap(layer1.kernel, "layer 1 - kernel");
	showMap(pool.a, "pool 1");
	showMap(layer2.net, "layer 2");
	showMap(pool2.a, "pool 2");
	showMap(std1.a);
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

//	test1();
	test2();


	std::exit(0);
}

