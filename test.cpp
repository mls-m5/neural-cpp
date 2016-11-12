/*
 * test.cpp
 *
 *  Created on: 22 aug 2016
 *      Author: mattias
 */

#define MATUNIT_DISABLE_ERROR_HANDLING

#include "unittest.h"
#include "network.h"

#include <iostream>

using namespace std;


TEST_SUIT_BEGIN

TEST_CASE("full layer test") {
	{
		ValueMap map(2, 1);
		map(0) = 1;
		map(1) = 1;
		InputLayer inputLayer(map);
		FullLayer layer(inputLayer, 1);
		layer.kernel.fill(1);
		layer.bias.fill(0);

		layer.forward();

		ASSERT_EQ(layer.net(0), 2);

		cout << layer.a[0] << endl;
	}
}



TEST_CASE("full layer simple backpropagation test") {
	{
		std::vector<std::vector<std::vector<ValueType>>> data= {
				{{0, 1}, {1}},
				{{1, 0}, {1}},
				{{0, 0}, {0}},
				{{1, 1}, {0}},
		};
		std::vector<TrainingSet> sets;

		for (auto it: data) {
			TrainingSet set;
			sets.push_back(TrainingSet(it[0], it[1]));
		}

		Network network(sets);
		auto outLayer = new FullLayer(network.back(), 1);
		network.pushLayer(outLayer);

		network.forwardPropagate();
		auto costBefore = network.getTotalCost();

		for (int i = 0; i < 60000; ++i) {
			network.backPropagationCycle();


			if (i % 1001 == 0) {
				//			cout << "======= Generation: " << i << " ======================" << endl;
				//			mn_forX(outLayer->d, x) {
				//				cout << "out: " << outLayer->a[x] << " vs " << network.getCurrentTrainingSet().output(x) << " = training" << endl;
				//				cout << "outLayer.d: " << outLayer->d[x] << endl;
				//			}
				//			cout << "total error (C²): " << network.getTotalCost() << endl;
			}
		}

		auto costAfter = network.getTotalCost();
		ASSERT_LT(costAfter, costBefore);
	}
	{
		std::vector<std::vector<std::vector<ValueType>>> data= {
				{{0}, {1, 0}},
				{{1}, {0, 1}},
		};
		std::vector<TrainingSet> sets;

		for (auto it: data) {
			TrainingSet set;
			sets.push_back(TrainingSet(it[0], it[1]));
		}

		Network network(sets);
		auto outLayer = new FullLayer(network.back(), 2);
		network.pushLayer(outLayer);

		network.forwardPropagate();
		auto costBefore = network.getTotalCost();

		for (int i = 0; i < 60000; ++i) {
			network.backPropagationCycle();


//			if (i % 1001 == 0) {
//				cout << "======= Generation: " << i << " ======================" << endl;
//				mn_forX(outLayer->d, x) {
//					cout << "out: " << outLayer->a[x] << " vs " << network.getCurrentTrainingSet().output(x) << " = training" << endl;
//					cout << "outLayer.d: " << outLayer->d[x] << endl;
//				}
//				cout << "total error (C²): " << network.getTotalCost() << endl;
//			}
		}

		auto costAfter = network.getTotalCost();
		ASSERT_LT(costAfter, costBefore);
	}
}


TEST_CASE("full layer four neuron bp test") {
	{
		std::vector<std::vector<std::vector<ValueType>>> data = {
				{{0, 0}, {0, 1}}, //Count
				{{0, 1}, {1, 0}},
				{{1, 0}, {1, 1}},
				{{1, 1}, {0, 0}},

//				{{0, 0}, {1, 1}}, //Inverted
//				{{0, 1}, {1, 0}},
//				{{1, 0}, {0, 1}},
//				{{1, 1}, {0, 0}},
		};
		std::vector<TrainingSet> sets;

		for (auto it: data) {
			TrainingSet set;
			sets.push_back(TrainingSet(it));
		}

		Network network(sets);
		auto hiddenLayer = new FullLayer(network.back(), 3);
		network.pushLayer(hiddenLayer);
		auto outLayer = new FullLayer(*hiddenLayer, 2);
		network.pushLayer(outLayer);

		network.forwardPropagate();
		auto costBefore = network.getTotalCost();

		for (int i = 0; i < 60000; ++i) {
			network.backPropagationCycle();


//			if (i % 1001 == 0) {
//				cout << "======= Generation: " << i << " ======================" << endl;
//				mn_forX(outLayer->d, x) {
//					cout << "out: " << outLayer->a[x] << " vs " << network.getCurrentTrainingSet().output(x) << " = training" << endl;
//					cout << "outLayer.d: " << outLayer->d[x] << endl;
//				}
//				cout << "total error (C²): " << network.getTotalCost() << endl;
//			}
		}

		auto costAfter = network.getTotalCost();
		ASSERT_LT(costAfter, costBefore / 2.); //Test so that the error is much lesser
	}
}

TEST_CASE("convolution layer test") {
	ValueMap inMap(2, 2, 1, {1, 0, 0, 1});
	ValueMap outMap(1, 1, 1, std::vector<float>({.5}));
	std::vector<TrainingSet> sets = {{inMap, outMap}};

	Network network(sets);

	auto layer = new ConvolutionLayer(network.back(), 1, 2);
	network.pushLayer(layer);

	network.backPropagationCycle();
	auto error = network.getTotalCost();

//	cout << "error in the beginning: " << error << endl;

	for (int i = 0; i < 1000; ++i) {
		network.backPropagationCycle();
	}


	auto afterError = network.getTotalCost();
//	cout << "error after: " << afterError << endl;

//	cout << "the final kernel:" << endl;
//	for (auto v: layer->kernel.data()) {
//		cout << v << endl;
//	}
//	cout << "--end of kernel" << endl;

	ASSERT_LT(afterError, error);
}

TEST_CASE("conv layer, learn blur-filter") {
	ValueMap inMap(10, 10);
	mn_forXY(inMap, x, y) {
		if ((x >= 5) xor (y >= 5)) {
			inMap(x, y) = 1;
		}
		else {
			inMap(x, y) = 0;
		}
	}
//	cout << "training input:" << endl;
//	inMap.printXY();
	ValueMap filteredMap(9, 9);
	filteredMap.fill(0);

	mn_forXY(filteredMap, x, y) {
		for (int ky = 0; ky < 2; ++ky) for (int kx = 0; kx < 2; ++kx) {
			filteredMap(x, y) += inMap(x +kx, y + ky) / 4.;
		}
	}
//	cout << "training output:" << endl;
//	filteredMap.printXY();

	std::vector<TrainingSet> sets = {{inMap, filteredMap}};

	Network network(sets);

	auto layer = new ConvolutionLayer(network.back(), 1, 2);
	network.pushLayer(layer);

	network.backPropagationCycle();
	auto error = network.getTotalCost();

//	cout << "error in the beginning: " << error << endl;

	for (int i = 0; i < 1000; ++i) {
		network.backPropagationCycle();
	}


	auto afterError = network.getTotalCost();
//	cout << "error after: " << afterError << endl;
//	cout << "final kernel:" << endl;
//	layer->kernel.printXY();
//
//	cout << "final output:" << endl;
//
//	network.back().a.printXY();

	ASSERT_LT(afterError, error);

}


TEST_SUIT_END

