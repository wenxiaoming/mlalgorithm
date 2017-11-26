#ifndef APRIORI_H
#define APRIORI_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

typedef struct {
    vector<int> data;
    int count;
} Attribute;

class Apriori
{
public:
    Apriori();
    ~Apriori();
    void loadDataSet(vector<string> dataset);
    void calculateFreqSet(float min);
    void createRules(float min);
private:
    void printVector(vector<int> data);
    void generateRulesFromConf(vector<int> currdata, vector<vector<int>> freqset, float min);
    vector<vector<int>> calcConf(vector<int> currdata, vector<vector<int>> freqset, float min);
    vector<vector<int>> generateNewSet(vector<vector<int>> mindata, int length);
    map<vector<int>, float> filterData(vector<vector<int>> currdata, vector<vector<int>>& mindata, float min);
    void updateData(vector<int> data);
    vector<int> getAttribute(string data);
    float mMinSupport;
    int mDataSetCount;
    vector<Attribute> mAttributeVec;
    vector<int> mDataSet;
    map<vector<int>, float> mFilterMap;
    vector<vector<vector<int>>> mFilterDataVec;
};

#endif
