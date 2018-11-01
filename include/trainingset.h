/*
 * trainingset.h
 *
 *  Created on: 29 okt. 2018
 *      Author: Mattias Larsson Sköld
 */


#pragma once


class TrainingSet {
public:
	template<class T>
	TrainingSet(std::vector<T> in, std::vector<T> out):
	input(in.size(), 1),
	output(out.size(), 1){
		for (size_t i = 0; i < in.size(); ++i) {
			input[i] = in[i];
		}
		for (size_t i = 0; i < out.size(); ++i) {
			output[i] = out[i];
		}
    };
	template<class T>
	TrainingSet(std::vector<std::vector<T>> set):
	TrainingSet(set[0], set[1])
	{}

	TrainingSet(const TrainingSet& set) {
		input = set.input;
		output = set.output;
	}
	TrainingSet() = default;

	TrainingSet(const ValueMap &in, const ValueMap &out):
	input(in), output(out){
	}
	ValueMap input;
	ValueMap output;
};



