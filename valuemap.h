/*
 * valuemap.h
 *
 *  Created on: 21 aug 2016
 *      Author: mattias
 */

#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "util.h"

typedef float ValueType;



#define mn_for1(bound,i) for (int i = 0; i<(int)(bound); ++i)
#define mn_forX(img,x) mn_for1((img)._width,x)
#define mn_forY(img,y) mn_for1((img)._height,y)
#define mn_forZ(img,z) mn_for1((img)._depth,z)
#define mn_forC(img,c) mn_for1((img)._spectrum,c)
#define mn_forXY(img,x,y) mn_forY(img,y) mn_forX(img,x)
#define mn_forXZ(img,x,z) mn_forZ(img,z) mn_forX(img,x)
#define mn_forYZ(img,y,z) mn_forZ(img,z) mn_forY(img,y)
#define mn_forXC(img,x,c) mn_forC(img,c) mn_forX(img,x)
#define mn_forYC(img,y,c) mn_forC(img,c) mn_forY(img,y)
#define mn_forZC(img,z,c) mn_forC(img,c) mn_forZ(img,z)
#define mn_forXYZ(img,x,y,z) mn_forZ(img,z) mn_forXY(img,x,y)
#define mn_forXYC(img,x,y,c) mn_forC(img,c) mn_forXY(img,x,y)
#define mn_forXZC(img,x,z,c) mn_forC(img,c) mn_forXZ(img,x,z)
#define mn_forYZC(img,y,z,c) mn_forC(img,c) mn_forYZ(img,y,z)
#define mn_forXYZC(img,x,y,z,c) mn_forC(img,c) mn_forXYZ(img,x,y,z)


template <class T = ValueType>
class GenericValueMap {
	typedef GenericValueMap<> ValueMap;
public:

	//Move constructor
	GenericValueMap(ValueMap &&from) {
		_data = std::move(from.data());
		_width = from._width;
		_height = from._height;
		_depth = from._height;
		_spectrum = from._spectrum;
	}

	//copy constructor
	template <class fromT>
	GenericValueMap(const fromT& m):
		GenericValueMap(m.width(), m.height(), m.depth(), m.spectrum())
	{
		_data = m.data();
	}
	GenericValueMap(int w = 1, int h = 1, int d = 1, int c = 1):
		_width(w),
		_height(h),
		_depth(d),
		_spectrum(c),
		_data(w * h * d * c){
	}

	GenericValueMap(int w, int h, int d, std::vector<ValueType> data):
		_width(w),
		_height(h),
		_depth(d),
		_spectrum(1),
		_data(data){

	}

	void setData(T *data) {
		mn_for1(_data.size(), i) {
			_data[i] = data[i];
		}
	}

	GenericValueMap(const GenericValueMap&) = default;

	template <class fromT>
	GenericValueMap<ValueType> &operator=(fromT &from) {
		_data = from.data();
		_width = from._width;
		_height = from._height;
		_depth = from._depth;
		_spectrum = from._spectrum;
		return *this;
	}

	GenericValueMap<> &operator=(const GenericValueMap<> & from) {
		_data = from.data();
		_width = from._width;
		_height = from._height;
		_depth = from._depth;
		_spectrum = from._spectrum;
		return *this;
	}

	int width() const {
		return _width;
	}

	int height() const {
		return _height;
	}

	int depth() const {
		return _depth;
	}

	int spectrum() const {
		return _spectrum;
	}

	int size() const {
		return _data.size();
	}

	const std::vector<T> &data() const {
		return _data;
	}

	std::vector<T> &data() {
		return _data;
	}

	//Create noise around zero
	void noise(T amplitude) {
		for (auto &it: _data) {
			it = rnd() * 2. * amplitude - 1.;
		}
	}

	void fill(T value) {
		for (auto &it: _data) {
			it = value;
		}
	}

	ValueType max() {
		auto m = -1000;
		for (auto it: _data) {
			if (it > m) {
				m = it;
			}
		}
		return m;
	}


	ValueType min() {
		auto m = 1000;
		for (auto it: _data) {
			if (it < m) {
				m = it;
			}
		}
		return m;
	}

	//Set the values to between 0 and 1
	ValueMap &normalze() {
		auto max = this->max();
		auto min = this->min();

		auto factor = 1. / (max - min);
		for (auto &it: _data) {
			it = (it - min) * factor;
		}
		return *this;
	}

	//Check if dimensions is the same
	bool isDimensionsSame(ValueMap &other) {
		if (other.width() != width() || other.height() != height() || other.depth() != depth() || other.spectrum() != spectrum()) {
			return false;
		}
		else {
			return true;
		}
	}

	T& operator()(const unsigned int x) {
		return _data[x];
	}

	const T& operator()(const unsigned int x) const {
		return _data[x];
	}

	T& operator[](const unsigned int x) {
		return _data[x];
	}

	const T& operator[](const unsigned int x) const {
		return _data[x];
	}

	T& operator()(const unsigned int x, const unsigned int y) {
		return _data[x + y*_width];
	}

	const T& operator()(const unsigned int x, const unsigned int y) const {
		return _data[x + y*_width];
	}

	T& operator()(const unsigned int x, const unsigned int y, const unsigned int z) {
		return _data[x + y*(size_t)_width + z*(size_t)_width*_height];
	}

	const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z) const {
		return _data[x + y*(size_t)_width + z*(size_t)_width*_height];
	}

	T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c) {
		return _data[x + y*(size_t)_width + z*(size_t)_width*_height + c*(size_t)_width*_height*_depth];
	}

	const T& operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int c) const {
		return _data[x + y*(size_t)_width + z*(size_t)_width*_height + c*(size_t)_width*_height*_depth];
	}

	std::string toString() {
		return "<ValueMap (" + i2str(width()) + ", " + i2str(height()) + ", " + i2str(depth()) + ", " + i2str(spectrum()) + ")>";
	}

	operator std::string() {
		return toString();
	}

	void printXY() {
		mn_forY(*this,y) {
			mn_forX(*this,x) {
				std::cout << this->operator() (x, y) << "\t";
			}
			std::cout << std::endl;
		}
	}

#define mn_unary_operator_definition(op) \
	ValueMap &operator op (ValueMap &map) { \
		for (size_t i = 0; i < _data.size(); ++i) { \
			_data[i] op map[i]; \
		} \
		return *this; \
	}

	mn_unary_operator_definition(-=);
	mn_unary_operator_definition(+=);
	mn_unary_operator_definition(/=);
	mn_unary_operator_definition(*=);


	//Pixelwise apply function
	ValueMap &apply(std::function<T(T&)> f, ValueMap &arg) {
		for (size_t i = 0; i < _data.size(); ++i) {
			_data[i] = f(arg[i]);
		}
		return *this;
	}

//protected: //some strange error shows up when trying to make these protected
	int _width;
	int _height;
	int _depth;
	int _spectrum; //Naming forth dimension

	//For debugging
	class BoundCheckingVector: public std::vector<T> {
	public:
		template<class V>
		BoundCheckingVector(V data): std::vector<T>(data) {}
		using std::vector<T>::vector;
		using std::vector<T>::operator=;
		BoundCheckingVector() = default;

		T& operator[](size_t index) {
			if (index > size()) {
				throw "index out of range";
			}
			return this->std::vector<T>::operator [](index);
		}
	};

	typedef BoundCheckingVector VectorType;
//	typedef std::vector<T> VectorType;

	VectorType _data;
};


typedef GenericValueMap<> ValueMap;
