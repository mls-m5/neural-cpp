/*
 * visual-app.cpp
 *
 *  Created on: 29 okt. 2018
 *      Author: Mattias Larsson Sköld
 */


#include <iostream>
#include "application.h"
#include "window.h"
#include "button.h"
#include "draw.h"
#include "texture.h"
#include "layer.h"
using namespace std;

//LIBS= ../matgui/matgui-sdl.a -lGL -lSDL2 -lSDL2_image -pthread

using namespace MatGui;

class ImageView: public View {
public:
	ImageView() {
		color.line.color(1, 1,1);
//		texture.load("../trainingsets/linus.png");

		data = loadMNISTBinary("../trainingsets/mnist");

	}

	void createTexture(int num) {
		auto tr = data.xtr[num];
		std::vector<Texture::Pixel> charValueMap(tr.size());

		mn_for1(tr.size(), i) {
			charValueMap[i].r = tr[i];
			charValueMap[i].g = tr[i];
			charValueMap[i].b = tr[i];
		}

		texture.createBitmap(charValueMap, tr.width(), tr.height());
	}

	void draw() {
		static int pictureNum = 0;
		static float t = 0;

		t += .1;
		if (t > 1) {
			t = 0;
			++pictureNum;
			createTexture(pictureNum);
		}


//		drawLine(0., 0., _width, _height, &color);
		drawTextureRect(0, 0, 0, _width, _height, texture, DrawStyle::OrigoTopLeft);
	}

	Paint color;
	Texture texture;
	TrainingData data;
};

int main(int argc, char **argv) {
	Application app(argc, argv);

	Window window("hej", 200, 200);
	window.currentStyle.line.color(1,1,1);

//	auto button = new Button("hej");
	auto image = new ImageView;

//	window.addChild(button);
	window.addChild(image);

	app.mainLoop();
	cout << "hej" << endl;
}

