#include "kmeans.h"

KMeans::KMeans()
{

}

KMeans::~KMeans()
{

}

void KMeans::loadDataSet(vector<string> dataSet)
{
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        vector<float> data = getAttribute(*iter);
        if (data.size() != 0) {
            mDataSet.push_back(data);
        }
    }
}

vector<float> KMeans::getAttribute(string dataStr)
{
    vector<float> retData;
    const char* str = dataStr.c_str();

    float value = 0.0;
    for (int index = 0; index < dataStr.size(); index++) {
        if (isspace(*str)) {
            str++;
            continue;
        }

        string s;
        while (!isspace(*str)) {
            s.insert(s.end(), *str);
            str++;
            index++;
            if (index >= dataStr.size())
                break;
        }
        value = atof(s.c_str());
        retData.push_back(value);
        str++;
    }
    return retData;
}

float KMeans::mean(vector<vector<float>> dataSet, int index)
{
    assert(index < dataSet[0].size());
    auto iter = dataSet.begin();
    float sum = 0.0f;
    for (; iter != dataSet.end(); iter++) {
        sum += (*iter)[index];
    }
    return sum/dataSet.size();
}

vector<float> KMeans::mean(vector<vector<float>> dataSet)
{
    vector<float> result;
    int index = 0;
    for(; index < dataSet[0].size(); index++) {
        auto iter = dataSet.begin();
        float sum = 0.0f;
        for (; iter != dataSet.end(); iter++) {
            sum += (*iter)[index];
        }
        result.push_back(sum/dataSet.size());
    }
    return result;
}


float KMeans::distance(vector<float> a, vector<float> b)
{
    float result = 0.0f;
    for (int i = 0; i < a.size(); i++) {
        float delta = abs(a[i] - b[i]);
        delta = pow(delta, 2);
        result += delta;
    }
    result += sqrt(result);
    return result;
}

#define random(x) (rand()%x)

vector<vector<float>> KMeans::findCenter(vector<vector<float>> dataSet, int num)
{
    vector<vector<float>> center;
    for (int i = 0; i < num; i++) {
        int index = random(dataSet.size());
        center.push_back(dataSet[index]);
    }
    return center;
}

vector<vector<float>> KMeans::filterCluster(vector<ErrorItem> clusterError, int clusterIndex, vector<int>& dataSetMap)
{
    //get the dataset which is in clusterIndex's cluster
    vector<vector<float>> dataSet;
    int index = 0;
    auto iter = clusterError.begin();
    for (; iter != clusterError.end(); iter++) {
        if ((*iter).cluster == clusterIndex) {
            dataSet.push_back(mDataSet[index]);
            dataSetMap.push_back(index);
        }

        index++;
    }
    return dataSet;
}

vector<ErrorItem> KMeans::findCluster(int clusterIndex, int splitNum, vector<vector<float>>& currCenter)
{
    vector<ErrorItem> clusterError;
    vector<int> dataSetMap;
    vector<vector<float>> dataSet = filterCluster(mClusterError, clusterIndex, dataSetMap);

    if (dataSet.size() == 0)
        return clusterError;

    //generate the new cluster for verifying
    currCenter = findCenter(dataSet, splitNum);

    //init clusterError
    for (int i = 0; i < dataSet.size(); i++) {
        ErrorItem item;
        item.index = dataSetMap[i];
        item.cluster = 0;
        item.error = 0.0f;
        clusterError.push_back(item);
    }

    bool change = true;
    while (change) {
        change = false;
        auto iter = clusterError.begin();
        for (int j = 0; j < dataSet.size(); j++) {
            float minDistance = INT32_MAX;
            int minClusetIndex = 0;
            for (int k = 0; k < splitNum; k++) {
                float dist = distance(dataSet[j], currCenter[k]);
                if (dist < minDistance) {
                    minDistance = dist;
                    minClusetIndex = k;
                }
            }

            if (minClusetIndex != (*iter).cluster) {
                change = true;
                (*iter).cluster = minClusetIndex;
            }
            (*iter).error = minDistance;

            iter++;
        }
        //calcuate the center
        for (int i = 0; i < splitNum; i++) {
            vector<int> newDataSetMap;
            vector<vector<float>> newDataSet = filterCluster(clusterError, i, newDataSetMap);

            if(newDataSet.size() != 0)
                currCenter[i] = mean(newDataSet);
        }
    }
    return clusterError;
}

int KMeans::findValidCluster()
{
    map<int, float> center;
    auto iter = mClusterError.begin();
    int index = 0;
    for (; iter != mClusterError.end(); iter++) {
        int index = (*iter).cluster;
        center[index] = (*iter).error;
    }
    return center.size();
}

void KMeans::createCluster(int clusterNum)
{
    vector<float> centerPoint;
    int num = mDataSet[0].size();
    for (int i = 0; i < num; i++) {
        centerPoint.push_back(mean(mDataSet, i));
    }

    vector<vector<float>> clusterList;
    clusterList.push_back(centerPoint);

    for (int i = 0; i < mDataSet.size(); i++) {
        float delta = distance(mDataSet[i], centerPoint);
        ErrorItem item;
        item.index = i;
        item.cluster = 0;
        item.error = delta;
        mClusterError.push_back(item);
    }

    int loop = 0;
    while ((loop < 1000) && (findValidCluster() < clusterNum)) {
        int clusterIndex = 0;
        float bestSSE = INT32_MAX;
        vector<vector<float>> bestSplitCenter;
        vector<ErrorItem> bestSplitClusterError;
        vector<int> bestDataSetMap;
        int bestSplitIndex = 0;
        for (; clusterIndex < clusterList.size(); clusterIndex++) {
            vector<vector<float>> currCenter;
            vector<ErrorItem> clusterError = findCluster(clusterIndex, 2, currCenter);

            if (clusterError.size() == 0)
                continue;

            float sseSplit = 0.0f;
            auto iter = clusterError.begin();
            for (; iter != clusterError.end(); iter++) {
                sseSplit += (*iter).error;
            }

            float sseNoSplit = 0.0f;
            iter = mClusterError.begin();
            for (; iter != mClusterError.end(); iter++) {
                if((*iter).cluster != clusterIndex)
                    sseNoSplit += (*iter).error;
            }

            if ((sseNoSplit + sseSplit) < bestSSE) {
                bestSSE = (sseNoSplit + sseSplit);
                bestSplitCenter = currCenter;
                bestSplitClusterError = clusterError;
                bestSplitIndex = clusterIndex;
            }
        }

        //rebuild mClusterError based on current split
        auto newiter = bestSplitClusterError.begin();
        int index = 0;
        for (; newiter != bestSplitClusterError.end(); newiter++) {
            int originIndex = (*newiter).index;//the index in the origin mClusterError
            if ((*newiter).cluster == 1) {
                mClusterError[originIndex].cluster = clusterList.size();
            } else {
                //(*newiter).cluster == 0
                mClusterError[originIndex].cluster = bestSplitIndex;
            }
        }
        //update clusterList
        clusterList[bestSplitIndex] = bestSplitCenter[0];
        clusterList.push_back(bestSplitCenter[1]);

        loop++;
    }
}

#define MAX_LINE 1024
void kmeans_test()
{
    KMeans* kmeans = new KMeans;

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

    kmeans->loadDataSet(dataSet);

    kmeans->createCluster(5);

    if (kmeans)
        delete kmeans;

    fclose(file);
}
