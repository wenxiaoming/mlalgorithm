#ifndef LOGISTIC_REGRESSION_H
#define LOGISTIC_REGRESSION_H

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include <math.h>

using namespace std;

class LogisticRegression
{
public:
    LogisticRegression();
    ~LogisticRegression();
    void loadDataSet(vector<string> dataSet);
    void train(int iterNum);
    float predict(string data);
private:
    vector<float> getAttribute(string dataStr, float& label);
    void stocGradAscent(int iterNum);
    float sum(vector<float> a, vector<float> b);
    int findRandomIndex(map<int, int> indexMap);
    float sigmoid(float value);
    vector<float> multiply(vector<float> a, float b);
    vector<float> add(vector<float> a, vector<float> b);
private:
    vector<vector<float>> mDataSet;
    vector<float> mDataLabel;
    int mDataSetCount;
    vector<float> mWeights;
};


#endif
