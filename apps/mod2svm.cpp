


#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "network.h"

using namespace std;

class SVM {
public:
    SVM(int numberOfLabels):
    W(numberOfLabels),
    bias(numberOfLabels)
    {
        for (auto &w: W) {
            w.resize(32,32,3);
            w.random();
        }
    }

    void train(ValueMapVector &Xtr, LabelVector &Ytr) {
        //Problem 1 hur använder man loss-funktionen för att träna upp lagret
    	//Jag tror att man gör det genom att man på något sätt räknar ut ett medelvärde för alla bilder

    	cout << "training support vector machine" << endl;

        for (size_t i = 0; i < W.size(); ++i) {
        	int numberInCategory = 0;
        	W[i].fill(0);
        	for (size_t j = 0; j < Xtr.size(); ++j) {
        		if ((size_t)Ytr[j] == i) {
        			++numberInCategory;
        			W[i] += Xtr[j];
        		}
        	}
        	W[i] /= numberInCategory; //Mean
        	bias[i] = W[i].abs();
        	W[i] *= (1. / bias[i]); //Normalize
//        	bias[i] = 0;
        }

        //Continue here
    }

    vector<int> predict(ValueMapVector &Xte) {
        vector <int> result(Xte.size());


//        ValueMap temp;

        for (size_t i = 0; i < Xte.size(); ++i) {
        	double loss = 1000000;
        	for (size_t j = 0; j < W.size(); ++j) {
        		double newLoss = abs(Xte[i].dot(W[j]) - bias[j]);
//        		temp = Xte[i];
//        		temp -= W[j];
//        		double newLoss = temp.sum();

        		if (newLoss < loss) {
        			loss = newLoss;
        			result[i] = j;
        		}
        	}
        }


        return result;

    }

    //Loss is called L in some examples
    float loss(ValueMap &x, unsigned int y) {
        float delta = 1.;
        // vector <float> scores(W.size());
        float lossResult = 0;


        float correctClassScore = W[y].dot(x) + bias[y];

        for (size_t i = 0; i < W.size(); ++i) {
            auto &w = W[i];
            auto score = w.dot(x) + bias[i];

            lossResult += (i != y) * max((float)0., score - correctClassScore + delta);
        }

        return lossResult;
    }

    //Same as loss with small letters but with vectors
    vector <float> Loss(ValueMapVector &X, LabelVector &Y) {
        vector <float> result(X.size());
        for (size_t i = 0; i < X.size(); ++i) {
            result[i] = loss(X[i], Y[i]);
        }
        return result;
    }

    ValueMapVector W;
    vector <float> bias;
};


void doSVM() {
    cout << "Starting SVM-test" << endl;

    SVM svm(10);

    vector<ValueMap> xte;
    vector<int> yte;

    {
        cout << "loading cfar10" << endl;
        auto data = loadCFAR10Binary("trainingsets/cifar-10-batches-bin/");
        cout << "loading complete" << endl;
        cout << "training data" << endl;
        svm.train(data.xtr, data.ytr);

        xte = move(data.xte);
        yte = move(data.yte);

        int numLabels = 0;
        for (int i = 0; i < (int)data.ytr.size(); ++i) {
        	if (data.ytr[i] > numLabels) {
        		numLabels = data.ytr[i];
        	}
        }

        for (int i = 0; i < numLabels; ++i) {
        	int c = 0;
        	for (int j = 0; j < (int)data.xtr.size(); ++j) {
        		c += (data.ytr[j] == i);
        	}
        	//cout << "numbers in category " << i << ": " << c << endl;
        }

        cout << "Xsamples: " << data.ytr.size() << ", etiketter " << numLabels + 1 << endl;
        cout << "Test samples: " << yte.size() << endl;

//        xte[3].prepare();
        showMap(xte[3], "SVM", false);

        showMap(svm.W[1], "mean");


        cout << "Test: " << xte[3].toString() << endl;
    }   

    cout << "predicting data" << endl;
    auto prediction = svm.predict(xte);

    size_t right = 0;

    for (size_t i = 0; i < prediction.size(); ++i) {
    	if (prediction[i] == yte[i]) {
    		++right;
    	}
    }

    double percentage = (float) right / prediction.size();

    cout << "Amount right: " << percentage << endl;


//    for (auto p: prediction) {
//        cout << "prediction " << p << endl;
//    }

    auto cost1 = svm.loss(xte[0], yte[0]);

    cout << "testing complete" << endl;
    cout << "cost first sample = " << cost1 << endl;

    // for (size_t i = 0; i < prediction.size() * 0 + 1000; ++i) {
    //  cout << i << ": " << data.yte[i] << " -> " << prediction[i] << endl;
    // }


    // data.second[0].printXY();
    keepOpen();
}



int main() {
    doSVM();
}
