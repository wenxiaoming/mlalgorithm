#include "logisticregression.h"

LogisticRegression::LogisticRegression()
{
    mDataSetCount = 0;
}

LogisticRegression::~LogisticRegression()
{

}

void LogisticRegression::loadDataSet(vector<string> dataSet)
{
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        float label = 0.0;
        vector<float> data = getAttribute(*iter, label);
        if (data.size() != 0) {
            mDataSet.push_back(data);
            mDataLabel.push_back(label);
        }
    }
    mDataSetCount += mDataSet.size();
}

float LogisticRegression::predict(string data)
{
    float realLabel;
    vector<float> convertedData = getAttribute(data, realLabel);

    float label = sigmoid(sum(convertedData, mWeights));

    if (label > 0.5)
        return 1.0f;
    else
        return 0.0f;
}

vector<float> LogisticRegression::getAttribute(string dataStr, float& label)
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
    //remove the last one since it is the label
    if (retData.end() != retData.begin())
        retData.erase(retData.end() - 1);
    label = value;

    return retData;
}

float LogisticRegression::sum(vector<float> a, vector<float> b)
{
    int i = 0;
    float result = 0.0f;
    for (; i < a.size(); i++) {
        result += a[i] * b[i];
    }
    return result;
}

vector<float> LogisticRegression::multiply(vector<float> a, float b)
{
    int i = 0;
    vector<float> result;
    for (; i < a.size(); i++) {
        float value = a[i] * b;
        result.push_back(value);
    }
    return result;
}

vector<float> LogisticRegression::add(vector<float> a, vector<float> b)
{
    int i = 0;
    vector<float> result;
    for (; i < a.size(); i++) {
        float value = a[i] + b[i];
        result.push_back(value);
    }
    return result;
}

float LogisticRegression::sigmoid(float value)
{
    return 1.0 / (1 + exp(-value));
}

#define random(x) (rand()%x)
int LogisticRegression::findRandomIndex(map<int, int> indexMap)
{
    auto iter = indexMap.begin();
    int randIndex = random(indexMap.size());

    while (indexMap[randIndex] == 1)//this index is removed, find the next one.
        randIndex = random(indexMap.size());

    return randIndex;
}

void LogisticRegression::stocGradAscent(int iterNum)
{
    for (int i = 0; i < iterNum; i++) {
        map<int, int> indexMap;//the first is the index, the second indicates the item is removed or not
        int j = 0;
        for (j = 0; j < mDataSetCount; j++) {
            indexMap[j] = 0;
        }

        for (j = 0; j < mDataSetCount; j++) {
            float alpha = 4/(1.0 + j + i) + 0.0001;
            int randIndex = findRandomIndex(indexMap);
            float h = sigmoid(sum(mDataSet[randIndex], mWeights));
            float error = mDataLabel[randIndex] - h;
            mWeights = add(mWeights, multiply(mDataSet[randIndex], alpha * error));
            indexMap[randIndex] = 1;
        }
    }
}

void LogisticRegression::train(int iterNum)
{
    //init weights;
    for (int i = 0; i < mDataSet[0].size(); i++)
        mWeights.push_back(1.0f);

    stocGradAscent(iterNum);
}

#define MAX_LINE 1024

void logisticregression_test()
{
    LogisticRegression* lg = new LogisticRegression();

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
    lg->loadDataSet(dataSet);

    lg->train(150);

    float label = lg->predict("1.0 2.1 2.50 0");
}
