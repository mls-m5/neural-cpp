//© Mattias Lasersköld
#include <iostream>
#include <string>
#include "network.h"
#include "gui.h"

using namespace std;


int main(int, char **) {
//	ValueMap inputMap(loadValueMapImg("trainingsets/linus.png", 100, 100));
//	ValueMap falseInputMap(loadValueMapImg("trainingsets/house.png", 100, 100));
//
////	inputMap.normalze();
//	input.prepare();
//
//	ValueMap catOutputMap(2, 1, 1, {1, 0}); //A cat
//	TrainingSet set1(inputMap, catOutputMap);
//
//	ValueMap falseOutput(2, 1, 1, {0, 1}); //A house is not a cat
//	TrainingSet set2(falseInputMap, falseOutput);
//
//	Network network({set1, set2});
//
//
//	//Create the chain och layers
//	auto layer1 = new ConvolutionLayer(network.back(), 5, 5);
//	layer1->name = "conv layer 1";
//	auto pool = new MaxPool(*layer1);
//	auto layer2 = new ConvolutionLayer(*pool, 5, 5);
//	layer2->name = "conv layer 2";
//	auto pool2 = new MaxPool(*layer2);
//	auto pool3 = new MaxPool(*pool2);
//	auto std1 = new FullLayer(*pool3, 10);
//	auto full2 = new FullLayer(*std1, 2);
//
//	network.setChain(full2);
//
//	size_t targetGeneration = 20;
//	for (size_t i = 0; i < targetGeneration; ++i) {
//		network.backPropagationCycle();
//		cout << "generation: " << i << "/" << targetGeneration << endl;
//		cout << "Totalt fel: " << network.getTotalCost() << endl;
//	}
//
//	showMap(inputMap);
//	showMap(layer1->net, "layer 1");
//	showMap(layer1->kernel, "layer 1 - kernel");
//	showMap(pool->a, "pool 1");
//	showMap(layer2->net, "layer 2");
//	showMap(layer2->kernel, "layer 2 - kernel");
//	showMap(pool2->a, "pool 2");
//	showMap(std1->a);
//	showMap(full2->a, "output");
//	keepOpen();
}
