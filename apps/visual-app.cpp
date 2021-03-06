/*
 * visual-app.cpp
 *
 *  Created on: 29 okt. 2018
 *      Author: Mattias Larsson Sköld
 */


#include <iostream>
#include <fstream>
#include "application.h"
#include "window.h"
#include "button.h"
#include "draw.h"
#include "texture.h"
#include "alllayers.h"
#include "network.h"
#include "imageview.h"
#include "label.h"
using namespace std;

//LIBS= ../matgui/matgui-sdl.a -lGL -lSDL2 -lSDL2_image -pthread

using namespace MatGui;

vector<TrainingSet> convertToTrainingSets(ValueMapVector x, LabelVector y) {
	vector<TrainingSet> ret;
	ret.reserve(x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		ValueMap res(10, 1, 1);
		res.fill(0);
		res[y[i]] = 1;
		ret.push_back({x[i], res});
	}

	return ret;
}

int getMaxIndex(const ValueMap &map) {
	float max = -1000000;
	float maxIndex = -1;
	for (size_t i = 0; i < map.size(); ++i) {
		if (map[i] > max) {
			max = map[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}

ofstream logFile("visual-app-logg.csv");

class TrainingView: public LinearLayout {
public:
	Paint color;
	ImageView *textureView, *internalView, *outputView;
	TrainingData data = loadMNISTBinary("../trainingsets/mnist");
	Label *resultLabel;
	vector<TrainingSet> sets = convertToTrainingSets(data.xtr, data.ytr);
	Network network;
	DenseLayer *fullLayer1 = nullptr;
	int generation = 0;

	TrainingView():
		network(sets)
	{
		setupNetwork();

		color.line.color(1, 1,1);

		logFile << "generation, percentage" << endl;

		addChild(textureView = new ImageView);
		textureView->weight(4);
		addChild(resultLabel = new Label("result here"));
		addChild(internalView = new ImageView);
		addChild(outputView = new ImageView);
	}

	void setupNetwork() {
		auto layer1 = new DenseLayer(network.back(), 40);
		this->fullLayer1 = layer1;
		auto output = new DenseLayer(*layer1, 10);
		network.setChain(output);

		network.forwardPropagate();
	}

	void createTexture() {
		auto map = network.layers.front()->a;
		map /= map.max();

		textureView->texture().createGrayscale(map.data(), map.width(), map.height());
		textureView->texture().interpolation(Texture::Nearest);

		auto a = fullLayer1->a;
		a /= a.max();
		internalView->texture().createGrayscale(a.data(), a.size(), 1);
		internalView->texture().interpolation(Texture::Nearest);

		auto outA = network.back().a.data();
		outputView->texture().createGrayscale(outA, outA.size());
		outputView->texture().interpolation(Texture::Nearest);
	}

	void draw() override {
		LinearLayout::draw();
		static float t = 2;
		static float right = 0;
		static float wrong = 0;
		int guessedIndex = 0;


		for (int i = 0; i < 100; ++i) {
			network.backPropagationCycle();
			cout << "generation " << generation << endl;
			cout << "totalt fel: " << network.getTotalCost() << endl;
			auto &output = network.layers.back()->a;
			//		cout << output << endl;
			int maxIndex = getMaxIndex(output);
			auto trueAnswer = getMaxIndex(network.getCurrentTrainingSet().output);
			guessedIndex = maxIndex;
			cout << "guess: " <<  maxIndex << "\t true:" << trueAnswer << endl;
			cout << "hit? " << (maxIndex == trueAnswer) << endl;

			if (maxIndex == trueAnswer) {
				right += 1;
			}
			else {
				wrong += 1;
			}

			float percentage = 100. * right / (right + wrong);

			cout << "percentage hit " << (int)(percentage) << "%" << endl;

			cout << endl;

			if (generation % 1000) {
				logFile << generation << ", " << percentage << endl;
			}


			++generation;
		}

		t += .1;
		if (t > .5) {
			t = 0;
			createTexture();
			resultLabel->label(to_string(guessedIndex));
		}
	}
};

int main(int argc, char **argv) {
	Application app(argc, argv);

	Window window("hej", 400, 400);
	window.currentStyle.line.color(1,1,1);

	auto image = new TrainingView;

	window.addChild(image);

	app.mainLoop();
}

