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
#include <memory>

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
		auto hiddenLayer = new FullLayer(network.back(), 3);
		auto outLayer = new FullLayer(*hiddenLayer, 1);
		network.setChain(outLayer);

		network.forwardPropagate();
		auto costBefore = network.getTotalCost();

		for (int i = 0; i < 60000; ++i) {
			network.backPropagationCycle();


//			if (i % 1001 == 0) {
//				cout << "======= Generation: " << i << " ======================" << endl;
//				cout << "error : " << network.getTotalCost() << endl;
//
//
//				cout << "\n---result: \n";
//				network.layers.back()->a.printXY();
//				cout << "target: \n";
//				network.getCurrentTrainingSet().output.printXY();
//			}
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

TEST_CASE("save test") {
	stringstream ss;

	ValueMap inputMap(4, 4, 1, 3);
	InputLayer input(inputMap);
	ConvolutionLayer c(input, 2, 3);
	FullLayer f(c, 5);

	c.kernel.noise(1);
	c.bias.noise(1);

	f.bias.noise(1);
	f.kernel.noise(1);

	ss << c;
	ss << f;
//	cout << ss.str() << endl;

	std::unique_ptr<Layer> c2ptr(Layer::load(ss, input));
	auto c2 = dynamic_cast<ConvolutionLayer*> (c2ptr.get());

	ASSERT(c2, "no layer or wrong type");
	ASSERT_EQ(c.kernel, c2->kernel);
	ASSERT_EQ(c.bias, c2->bias);


	std::unique_ptr<Layer> f2ptr(Layer::load(ss, *c2));
	auto f2 = dynamic_cast<FullLayer*> (f2ptr.get());


	ASSERT(f2, "no layer or wrong type");
	ASSERT_EQ(f.kernel, f2->kernel);
	ASSERT_EQ(f.bias, f2->bias);
}


TEST_CASE("dream test") {
	ValueMap input(2, 2, 1, {1, 0, 1, 1});
	ValueMap output(3, 1, 1, {0, 1, 0});

	Network network({{input, output}});

	network.pushLayer(new FullLayer(network.back(), 3));

	auto errorBefore = network.getTotalCost();
	cout << "Error before: " << errorBefore << endl;

	for (int i = 0; i < 10; ++i) {
		network.dreamCycle();
//		cout << "error: " << network.getTotalCost() << endl;
	}

	auto errorAfter = network.getTotalCost();
	cout << "Error after: " << errorAfter << endl;

	network.inputLayer().a.printXY();
	ASSERT_LT(errorAfter, errorBefore);

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
//			cout << "\n---result: \n";
//			network.layers.back()->a.printXY();
//			cout << "target: \n";
//			network.getCurrentTrainingSet().output.printXY();
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

TEST_CASE("valuemap reshape test") {
	ValueMap map1(2, 2, 1, {1, 2, 3, 4});
	ValueMap map2(4, 1, 1, {1, 2, 3, 4});

	map1.printXY();
	map2.printXY();

	map2.reshape(2, 2);
	map2.printXY();

	ASSERT_EQ(map1.width(), map2.width());
	ASSERT_EQ(map1.height(), map2.height());

	for (size_t i = 0; i < map1.size(); ++i) {
		ASSERT_EQ(map1[i], map2[i]);
	}
}


TEST_CASE("max pool") {
	ValueMap map1(2, 2, 1, {1, 2, 3, 4});
	InputLayer input(map1);
	MaxPool pool(input);
	pool.forward();

	ASSERT_EQ(pool.fromIndex(0, 0), 3);
	ASSERT_EQ(pool.a(0 , 0), 4);
}


TEST_SUIT_END

