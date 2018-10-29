

#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include "gui.h"
#include "network.h"

using namespace std;


class NearestNeighbour {
public:

    void train(vector<ValueMap> &xtr, vector<int> &ytr) {
        this->xtr = xtr;
        this->ytr = ytr;
    }

    vector<int> predict(vector<ValueMap> &xte, vector<int> &right) {
        vector <int> result(xte.size());

        long correct = 0, incorrect = 0;

        for (size_t i = 0; i < xte.size(); ++i) {
            auto &xi = xte[i];
            float distance = 32*32*3*256;
            long nearestIndex = -1;
            for (size_t j = 0; j < xtr.size(); ++j) {
                // cout << "test j = " << j << endl;
                long dlong = 0;
                auto &xj = xtr[j];
                mn_for1(xi.size(), px) {
                    dlong += abs(xj(px) - xi(px));
                }
                // cout << dlong << endl;

                float d = (float) dlong;

                if (d < distance) {
                    nearestIndex = j;
                    distance = d;
                }

            }

            int label = ytr[nearestIndex];

            if (label == right[i]) {
                ++correct;
            }
            else {
                ++incorrect;
            }

            cout << "test i = " << i << endl;
            cout << "result :" << label << "\tright:" << right[i] << endl;
            cout << "percentage: " << 100 * ((float)correct / (i + 1)) << "%" << endl;
            result.push_back(nearestIndex);
        }
        return result;
    }

    vector<ValueMap> xtr;
    vector<int> ytr;
};


void testNearestNeighbour() {


    // for (int i = 0; i < 100; ++i) {
    //  cout << data.yte[i] << endl;
    // }

    NearestNeighbour nn;


    vector<ValueMap> xte;
    vector<int> yte;

    {
        cout << "loading cfar10" << endl;
        auto data = loadCFAR10Binary("trainingsets/cifar-10-batches-bin/");
        cout << "loading complete" << endl;
        cout << "training data" << endl;
        nn.train(data.xtr, data.ytr);

        xte = move(data.xte);
        yte = move(data.yte);

        showMap(xte[2], "nearest neighbour", false);
    }   

    cout << "predicting data" << endl;
    auto prediction = nn.predict(xte, yte);

    cout << "testing complete" << endl;

    // for (size_t i = 0; i < prediction.size() * 0 + 1000; ++i) {
    //  cout << i << ": " << data.yte[i] << " -> " << prediction[i] << endl;
    // }


    // data.second[0].printXY();
    keepOpen();
}



int main() {
    testNearestNeighbour();
}