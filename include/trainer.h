/*
 * trainer.h
 *
 *  Created on: 29 okt. 2018
 *      Author: Mattias Larsson Sköld
 */


#pragma once


#include <vector>
#include <map>
#include "trainingset.h"


class Trainer {
public:
	virtual ~Trainer() {}

	virtual void rotate() = 0;

	virtual TrainingSet &getCurrent() = 0;
};

class LinearTrainer: public Trainer {
public:
	std::vector<TrainingSet> _sets;
	size_t _current = 0;

	LinearTrainer(std::vector<TrainingSet> trainingSets) {
		_sets = trainingSets;
	}

	LinearTrainer() = default;
	LinearTrainer(const LinearTrainer &) = default;
	LinearTrainer(LinearTrainer &&) = default;

	void rotate() override {
		if (++_current >= _sets.size()) {
			_current = 0;
		}
	}

	TrainingSet &getCurrent() override {
		return _sets[_current];
	}

	void add(TrainingSet set) {
		_sets.push_back(std::move(set));
	}
};

//Order the training sets after which output that is the strongest
//Good for pattern recognition training
class PatternTrainer: public Trainer {
protected:
	std::map<int, LinearTrainer> _subTrainers;
	size_t _current = 0;
public:


	PatternTrainer(std::vector<TrainingSet> trainingSets) {
		for (auto &it: trainingSets) {
			add(it);
		}
	}

	PatternTrainer() = default;
	PatternTrainer(const PatternTrainer &) = default;
	PatternTrainer(PatternTrainer &&) = default;

	void rotate() override {
		if (++_current >= _subTrainers.size()) {
			_current = 0;
		}
		_subTrainers[_current].rotate();
	}

	TrainingSet &getCurrent() override {
		return _subTrainers[_current].getCurrent();
	}


	void add(TrainingSet set) {
		if (set.output.height() != 1 || set.output.depth() != 1) {
			throw std::range_error("Trainingset height and depth must be 1 for PatternTrainer");
		}

		ValueType max = -10000000;
		int maxNumber = -1;
		for (auto i = 0; i < set.output.width(); ++i) {
			if (set.output(i) > max) {
				max = set.output(i);
				maxNumber = i;
			}
		}
		_subTrainers[maxNumber].add(set);
	}

};

