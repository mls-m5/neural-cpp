/*
 * convolutionlayer.h
 *
 *  Created on: 8 dec. 2018
 *      Author: Mattias Larsson Sköld
 */


#pragma once

#include "layer.h"


class ConvolutionLayer: public Layer {
public:
	ValueMap net; //The summed input (z)
	ValueMap kernel; //The weight of the connections, shared
	ValueMap bias; //The bias of the connections, also shared

	ConvolutionLayer(int width, int height, int depth, int kernelSize, int kernelDepth):
	Layer(width, height, depth),
	net(width, height, depth),
	kernel(kernelSize, kernelSize, kernelDepth, depth),
	bias(kernelSize, kernelSize, kernelDepth, depth){
		type = "conv";
		kernel.noise(1);
		bias.noise(1);
	}

	ConvolutionLayer(const ConvolutionLayer&) = delete;

	ConvolutionLayer(std::istream &stream, Layer &input):
		kernel(stream),
		bias(stream) {
		type = "conv";
		auto &ia = input.a;
		resize(ia.width(), ia.height(), ia.depth());
	}

	ConvolutionLayer(Layer &input, int depth, int kernelSize):
	ConvolutionLayer(input.a.width() - kernelSize + 1,
	input.a.height() - kernelSize + 1, depth, kernelSize, input.a.depth()){
		setSource(input);
	}

	void save(std::ostream &stream) override {
		saveHeader(stream);
		kernel.save(stream);
		bias.save(stream);
	}

	void forward() override {
		net.fill(0);
		//Not that the kz does not work in the same convolving way as the others
		//All layers in the output layer should be connected to all the neuron in
		//z-axis but not in the other axes
		//Also note that  the kernel has four dimensions 3 for the output layer and 1 (c) for the input layer
		//That is because the kernel is shared for all the neurons in the output layers xy axis (0 dimensions)
		//but not for the z axis (1 dimension)
		mn_forXYZ(net, x, y, z) {
			ValueType n = 0;
			mn_forXYZ(kernel, kx, ky, kz) {
				auto w = kernel(kx, ky, kz, z); //Note that the last dimension is the separated z axis
				n += source->a(x + kx, y + ky, kz) * w + bias(kx, ky, kz, z);
			}
			net (x, y, z) = n;
		}

//		mn_forXYZ(a, x, y, z) {
//			auto n = net(x, y, z);
//			a(x, y, z) = sigmoidFunction(n); // Add activation function
//			aPrim(x, y, z) = sigmoidDerivate(n); //Calculate derivative, this can be optimized
//		}

		activationFunction(activationFunctionType(), net, a, aPrim);
	}

	void prepareBackward() override {
		d.fill(0);
	}

	void backward() override {
		auto &sourceD = source->d;
		mn_forXYZ(d, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				//Like the forward propagation above but backwards (and the derivative so there is no bias)
				sourceD(x + kx, y + ky, kz) += d(x, y, z) * aPrim(x, y, z) * kernel(kx, ky, kz, z);
			}
		}
	}

	//Call for hidden and output layers
	void correctErrors(ValueType learningRate) override {
		auto &sourceA = source->a; //I have no idea, check so that it works
//		mn_forXYZ(bias, x, y, z) {
//			bias(x, y, z) -= d(x, y, z) * learningRate;
//		}

		double scale = 1. / kernel.data().size();
		mn_forXYZ(d, x, y, z) {
			mn_forXYZ(kernel, kx, ky, kz) {
				bias(kx, ky, kz, z) -= d(x, y, z) * learningRate * scale;
				kernel(kx, ky, kz, z) -= d(x, y, z) * sourceA(x, y, kz) * learningRate * scale;
			}
		}

		//Prototype from fully connected layer
//		auto &sourceA = source->a;
//		mn_forX(bias, x) {
//			bias(x) -= d(x) * learningRate;
//		}
//		mn_forX(net, ox) {
//			mn_forXYZ(kernel, x, y, z) {
//				kernel(x, y, z, ox) -= this->d(ox) * sourceA(x, y, z) * learningRate;
//			}
//		}
	}
};

