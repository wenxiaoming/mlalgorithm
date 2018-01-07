#ifndef KMEANS_H
#define KMEANS_H

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

typedef struct {
    int index;
    int cluster;
    float error;
} ErrorItem;

class KMeans
{
public:
    KMeans();
    ~KMeans();
public:
    void loadDataSet(vector<string> dataSet);
    void createCluster(int clusterNum);
private:
    int findValidCluster();
    vector<vector<float>> filterCluster(vector<ErrorItem> clusterError, int clusterIndex, vector<int>& dataSetMap);
    vector<vector<float>> findCenter(vector<vector<float>> dataSet, int num);
    vector<ErrorItem> findCluster(int clusterIndex, int splitNum, vector<vector<float>>& currCenter);
    float mean(vector<vector<float>> dataSet, int index);
    vector<float> mean(vector<vector<float>> dataSet);
    float distance(vector<float> a, vector<float> b);
    vector<float> getAttribute(string dataStr);
private:
    vector<vector<float>> mDataSet;
    vector<ErrorItem> mClusterError;
};

#endif