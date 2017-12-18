#ifndef SVM_H
#define SVM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <math.h>

using namespace std;

#define random(x) (rand()%x)

class SVM
{
public:
    SVM(float margin, float slack, int loop, string kernel, float kernelParam);
    ~SVM();
    void loadDataSet(vector<string> dataSet);
    void train();
    void predict();
private:
    void printSVMInfo();
    float clip(float value, float high, float low);
    vector<float> minus(vector<float> a, vector<float> b);
    vector<float> multiply(vector<float> a, vector<float> b);
    float dot(vector<float> a, vector<float> b);
    vector<float> getAttribute(string dataStr, float& label);
    float calcError(int index);
    int selectAnotherIndex(int index, float error, float& nextError);
    int checkExample(int indexI);
    float runKernel(vector<float> x, vector<float> y);
private:
    float mMargin;
    float mSlack;
    int mMaxLoop;
    string mKernelName;
    float mKernelParam;
    vector<vector<float>> mDataSet;
    vector<float> mDataLabel;
    int mDataSetCount;
    vector<float> mAlpha;//lagrange multipliers
    float mBias;//bias parameter
    map<int, float> mErrorCache; //error cache for SMO, the fist is example's index, the second is the error
};

#endif