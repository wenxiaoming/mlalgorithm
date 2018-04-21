#ifndef ADABOOST_H
#define ADABOOST_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

enum SplitDirection {
    LESS_THAN,
    BIG_THAN,
    DIRECTION_NUM
};

typedef struct {
    int index;
    float thresh;
    float alpha;
    SplitDirection dir;
} Stump;

class AdaBoost
{
public:
    AdaBoost();
    ~AdaBoost();
    void loadDataSet(vector<string> dataset);
    void train(int iter);
    void predict(vector<string> dataset);
private:
    vector<float> calcError(vector<float> verifyResult);
    void findMinMax(int attrIndx, float& min, float& max);
    Stump createStump(float& minError, vector<float>& bestPredict);
    vector<float> verifyStump(int attrIndex, int thresh, SplitDirection direction);
    int mDataSetCount;
    vector<vector<float>> mDataSet;
    vector<float> mLabel;
    vector<float> mWeight;
    int mStepNum;
    vector<Stump> mWeakClass;
};

#endif
