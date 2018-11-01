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
#include "layer.h"
#include "network.h"
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

class ImageView: public LinearLayout {
public:
	Paint color;
	Texture texture;
	TrainingData data = loadMNISTBinary("../trainingsets/mnist");
	vector<TrainingSet> sets = convertToTrainingSets(data.xtr, data.ytr);
	Network network;
	int generation = 0;

	ImageView():
		network(sets)
	{
		setupNetwork();

		color.line.color(1, 1,1);

		logFile << "generation, percentage" << endl;
	}

	void setupNetwork() {
		auto layer1 = new FullLayer(network.back(), 40);
		auto output = new FullLayer(*layer1, 10);
		network.setChain(output);

		network.forwardPropagate();
	}

	void createTexture(const ValueMap map) {
		auto tr = map;
		std::vector<Texture::Pixel> charValueMap(tr.size());

		mn_for1(tr.size(), i) {
			charValueMap[i].r = tr[i];
			charValueMap[i].g = tr[i];
			charValueMap[i].b = tr[i];
		}

		texture.createBitmap(charValueMap, tr.width(), tr.height());
	}

	void draw() {
		static int pictureNum = -1;
		static float t = 2;
		static float right = 0;
		static float wrong = 0;

		t += .1;
		if (t > 1) {
			t = 0;
			++pictureNum;
			createTexture(data.xtr[pictureNum]);
		}


		for (int i = 0; i < 100; ++i) {
			network.backPropagationCycle();
			cout << "generation " << generation << endl;
			cout << "totalt fel: " << network.getTotalCost() << endl;
			auto &output = network.layers.back()->a;
			//		cout << output << endl;
			int maxIndex = getMaxIndex(output);
			auto trueAnswer = getMaxIndex(network.getCurrentTrainingSet().output);
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

		drawTextureRect(0, 0, 0, _width, _height, texture, DrawStyle::OrigoTopLeft);
	}
};

int main(int argc, char **argv) {
	Application app(argc, argv);

	Window window("hej", 400, 400);
	window.currentStyle.line.color(1,1,1);

	auto image = new ImageView;

	window.addChild(image);

	app.mainLoop();
}

