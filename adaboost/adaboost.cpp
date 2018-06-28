#include <limits.h>
#include <math.h>
#include "adaboost.h"
#include "../common/math.h"
#include "../common/dataset.h"

AdaBoost::AdaBoost()
{
    mDataSetCount = 0;
    mStepNum = 10;
}

AdaBoost::~AdaBoost()
{

}

void AdaBoost::loadDataSet(vector<string> dataSet)
{
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        vector<float> data = getAttribute(*iter);
        if (data.size() != 0) {
            vector<float> attrData;
            attrData.assign(data.begin(), data.end() - 1);
            mDataSet.push_back(attrData);
            mLabel.push_back(data[data.size() - 1]);
        }
    }

    mDataSetCount = mDataSet.size();
}

void AdaBoost::train(int iter)
{
    int num = mDataSetCount;
    fill_n(back_inserter(mWeight), num, 1.0 / num);
    vector<float> aggPredict;
    fill_n(back_inserter(aggPredict), num, 0.0);

    for (int i = 0; i < iter; i++) {
        float minError;
        vector<float> bestPredict;
        Stump bestStump = createStump(minError, bestPredict);
        //update alpha
        float alhpa = float(0.5*log((1.0 - minError) / minError));
        printf("index:%d minError:%f alhpa:%f \n", i, minError, alhpa);
        bestStump.alpha = alhpa;
        mWeakClass.push_back(bestStump);
        //update mWeight;
        vector<float> exponArray = dotproduct(multiply(mLabel, -1*alhpa), bestPredict);
        for (int j = 0; j < mWeight.size(); j++) {
            mWeight[j] = mWeight[j]* exp(exponArray[j]);
        }
        normalize(mWeight);

        aggPredict = add(aggPredict, multiply(bestPredict, alhpa));
        vector<float> aggErrors = comparesign(aggPredict, mLabel);
        float errorRate = sum(aggErrors)/mDataSetCount;
        printf("errorRate:%f \n", errorRate);
        if (errorRate == 0.0)
            break;
    }
}

void AdaBoost::predict(vector<string> dataSet)
{
    //load dataset
    vector<vector<float>> testDataSet;
    vector<float> testLabel;
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        vector<float> data = getAttribute(*iter);
        if (data.size() != 0) {
            vector<float> attrData;
            attrData.assign(data.begin(), data.end() - 1);
            testDataSet.push_back(attrData);
            testLabel.push_back(data[data.size() - 1]);
        }
    }

    //predict
    vector<float> predictResult;
    fill_n(back_inserter(predictResult), testDataSet.size(), 0);

    for (int i = 0; i < mWeakClass.size(); i++) {
        int index = mWeakClass[i].index;
        int thresh = mWeakClass[i].thresh;
        SplitDirection dir = mWeakClass[i].dir;

        vector<float> verifyResult = verifyStump(index, thresh, dir, testDataSet, testDataSet.size());
        predictResult = add(predictResult, multiply(verifyResult, mWeakClass[i].alpha));
    }

    //compare with the label
    vector<float> predictErrors = comparesign(predictResult, testLabel);
    float errorRate = sum(predictErrors) / testDataSet.size();
    printf("predict errorRate:%f \n", errorRate);
}

void AdaBoost::findMinMax(int attrIndx, float& min, float& max)
{
    min = INT_MAX;
    max = -INT_MAX;

    for (int i = 0; i < mDataSetCount; i++) {
        if (mDataSet[i][attrIndx] > max)
            max = mDataSet[i][attrIndx];

        if (mDataSet[i][attrIndx] < min)
            min = mDataSet[i][attrIndx];
    }
}

vector<float> AdaBoost::calcError(vector<float> verifyResult)
{
    vector<float> error;
    fill_n(back_inserter(error), mDataSetCount, 1);
    for (int i = 0; i < mDataSetCount; i++) {
        if (mLabel[i] == verifyResult[i])
            error[i] = 0.0f;
    }

    return error;
}

Stump AdaBoost::createStump(float& minError, vector<float>& bestPredict)
{
    int attrNum = mDataSet[0].size();
    Stump bestStump;
    minError = INT_MAX;
    //loop all attribute
    for (int i = 0; i < attrNum; i++) {
        //loop ith attribute's value with step
        float rangeMin = 0.0f;
        float rangeMax = 0.0f;
        findMinMax(i, rangeMin, rangeMax);
        float step = (rangeMax - rangeMin) / mStepNum;

        for (int j = -1; j < mStepNum + 1; j++) {
            //loop two directions
            for (int dir = 0; dir < DIRECTION_NUM; dir++) {
                float thresh = rangeMin + float(j)*step;
                vector<float> verifyResult = verifyStump(i, thresh, (SplitDirection)dir, mDataSet, mDataSetCount);
                vector<float> verifyError = calcError(verifyResult);

                float error = multiply(verifyError, mWeight);
                if (error < minError) {
                    minError = error;
                    bestStump.index = i;
                    bestStump.dir = (SplitDirection)dir;
                    bestStump.thresh = thresh;
                    bestPredict.assign(verifyResult.begin(), verifyResult.end());
                }
            }
        }
    }
    return bestStump;
}

vector<float> AdaBoost::verifyStump(int attrIndex, int thresh, SplitDirection direction, vector<vector<float>> dataSet, int dataNum)
{
    vector<float> verifyResult;
    fill_n(back_inserter(verifyResult), dataNum, 1);
    for (int i = 0; i < dataNum; i++) {
        if (direction == LESS_THAN) {
            if (dataSet[i][attrIndex] < thresh)
                verifyResult[i] = -1;
        } else {
            if (dataSet[i][attrIndex] > thresh)
                verifyResult[i] = -1;
        }
    }

    return verifyResult;
}


#define MAX_LINE 1024

void adaboost_test()
{
    AdaBoost* adaboost = new AdaBoost;
    if(adaboost == NULL)
        return;
    //update dataset
    FILE* file = fopen("dataset.txt", "rt");

    if (file == NULL) {
        printf("fail to open dataset.txt\n");
        return;
    }

    vector<string> dataSet;
    char strLine[MAX_LINE];
    while (!feof(file)) {
        memset(strLine, 0, MAX_LINE);
        fgets(strLine, MAX_LINE, file);
        string s(strLine);
        dataSet.push_back(s);
    }

    fclose(file);
    adaboost->loadDataSet(dataSet);
    adaboost->train(10);

    //load test dataset
    file = fopen("testdataset.txt", "rt");
    if (file == NULL) {
        printf("fail to open testdataset.txt\n");
        return;
    }

    vector<string> testDataSet;
    while (!feof(file)) {
        memset(strLine, 0, MAX_LINE);
        fgets(strLine, MAX_LINE, file);
        string s(strLine);
        testDataSet.push_back(s);
    }

    adaboost->predict(testDataSet);

    fclose(file);
    delete adaboost;
}
