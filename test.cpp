/*
 * test.cpp
 *
 *  Created on: 22 aug 2016
 *      Author: mattias
 */


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

		cout << layer.a[0];
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


TEST_CASE("full layer four neuron backpropagation test") {
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


TEST_SUIT_END

