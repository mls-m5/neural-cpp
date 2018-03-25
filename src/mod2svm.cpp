


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
    }

    vector<int> predict(ValueMapVector &Xte) {
        vector <int> result;

        for (size_t i = 0; i < Xte.size(); ++i) {

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

        showMap(xte[3], "SVM", false);
    }   

    cout << "predicting data" << endl;
    auto prediction = svm.predict(xte);

    for (auto p: prediction) {
        cout << "prediction " << p << endl;
    }

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