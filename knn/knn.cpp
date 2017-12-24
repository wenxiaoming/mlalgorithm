#include "knn.h"

KNN::KNN(int k)
{
    mSelectNum = k;
}

KNN::~KNN()
{

}

void KNN::loadDataSet(vector<string> dataSet)
{
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        vector<float> data = getAttribute(*iter);
        if (data.size() != 0) {
            mDataSet.push_back(data);
        }
    }
}

void KNN::loadLabel(vector<string> label)
{
    mDataLabel = label;
}

float KNN::distance(vector<float> a, vector<float> b)
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

string KNN::predict(string data)
{
    vector<float> predictData = getAttribute(data);
    map<int, float> distMap;
    for (int i = 0; i < mDataSet.size(); i++) {
        distMap[i] = distance(mDataSet[i], predictData);
    }

    //sort the distance
    vector<PAIR> sortedMap(distMap.begin(), distMap.end());
    sort(sortedMap.begin(), sortedMap.end(), cmpbyvalue());

    map<string,int> labelMap;
    auto iter = sortedMap.begin();
    for (int j = 0; j < mSelectNum; j++) {
        string label = mDataLabel[iter->first];
        labelMap[label]++;
        iter++;
    }
    //get the k nearest label
    auto labeliter= labelMap.begin();
    string mostLabel;
    int max = 0;
    for (; labeliter != labelMap.end(); labeliter++) {
        if (labeliter->second > max) {
            max = labeliter->second;
            mostLabel = labeliter->first;
        }
    }
    return mostLabel;
}

vector<float> KNN::getAttribute(string dataStr)
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
void knn_test()
{
    KNN* knn = new KNN(5);

    vector<string> dataSet;
    dataSet.push_back("2  3  7  6  3");
    dataSet.push_back("4  2  6  7  4");
    dataSet.push_back("5  3  4  7  5");
    dataSet.push_back("6  7  9  10 5");

    dataSet.push_back("1  2  4  5  2");
    dataSet.push_back("2  4  5  6  8");
    dataSet.push_back("5  2  7  8  4");

    knn->loadDataSet(dataSet);

    vector<string> label;
    label.push_back("low");
    label.push_back("low");
    label.push_back("middle");
    label.push_back("middle");
    label.push_back("high");
    label.push_back("high");
    label.push_back("high");

    knn->loadLabel(label);

    string result = knn->predict("2  2  7  6  4");

    printf("the label for 2  2  7  6  4 is:%s \n", result.c_str());

    delete knn;
}
