#ifndef NAIVEBAYES_H
#define NAIVEBAYES_H

#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

typedef struct _Attribute {
    int attrvalue;
    int count;
    _Attribute() {
        attrvalue = 0;
        count = 0;
    }
} Attribute;

class naivebayes
{
public:
    naivebayes(int num);
    ~naivebayes();
    void loadPositiveSet(vector<string> trainSet);
    void loadNegetiveSet(vector<string> trainSet);
    void train();
    void classify(string testSet);
private:
    vector<int> getAttribute(string dataStr);
    void splitSet(string dataStr, vector<vector<Attribute>>& attrArray);
    void printDataSet(vector<vector<Attribute>> attrArray);
    vector<string> mTrainSet;
    vector<int> mLabelSet;
    int mAttributeNum;
    vector<vector<Attribute>> mPositiveAttrArray;
    vector<vector<Attribute>> mNegitiveAttrArray;
    float mPositiveProb;//probability of positive dataset
    float mNegetiveProb;//probability of positive dataset
};

#endif