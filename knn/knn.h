#ifndef KNN_H
#define KNN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <math.h>

using namespace std;

typedef pair<int, float> PAIR;
struct cmpbyvalue {
    bool operator()(const PAIR& lhs, const PAIR& rhs) {
        return lhs.second < rhs.second;
    }
};

class KNN
{
public:
    KNN(int k);
    ~KNN();
    void loadDataSet(vector<string> dataSet);
    void loadLabel(vector<string> label);
    string predict(string data);
private:
    float distance(vector<float> a, vector<float> b);
    vector<float> getAttribute(string dataStr);
private:
    int mSelectNum;
    vector<vector<float>> mDataSet;
    vector<string> mDataLabel;
};

#endif