#include "svm.h"

SVM::SVM(float margin, float slack, int loop, string kernel, float kernelParam)
{
    mMargin = margin;
    mSlack = slack;
    mMaxLoop = loop;
    mKernelName = kernel;
    mKernelParam = kernelParam;
}

SVM::~SVM()
{

}

void SVM::loadDataSet(vector<string> dataSet)
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

vector<float> SVM::getAttribute(string dataStr, float& label)
{
    vector<float> retData;
    const char* str = dataStr.c_str();

    float value = 0.0;
    for(int index = 0; index < dataStr.size(); index++) {
        if (isspace(*str)) {
            str++;
            continue;
        }

        string s;
        while(!isspace(*str)) {
            s.insert(s.end(), *str);
            str++;
            index++;
            if(index >= dataStr.size())
                break;
        }
        value = atof(s.c_str());
        retData.push_back(value);
        str++;
    }

    //remove the last one since it is the label
    if(retData.end() != retData.begin())
        retData.erase(retData.end()-1);
    label = value;

    return retData;
}

float SVM::runKernel(vector<float> x, vector<float> y)
{
    if (mKernelName.compare("rbf") == 0) {
        vector<float> k = minus(x, y);
        float result = dot(k, k);
        result = exp(result / (-1 * pow(mKernelParam, 2)));
        return result;
    } else
        printf("not supported \n");

    return 0.0f;
}

vector<float> SVM::minus(vector<float> a, vector<float> b)
{
    int i = 0;
    vector<float> result;
    for(; i < a.size(); i++) {
        float value = a[i]-b[i];
        result.push_back(value);
    }
    return result;
}

vector<float> SVM::multiply(vector<float> a, vector<float> b)
{
    int i = 0;
    vector<float> result;
    for(; i < a.size(); i++) {
        float value = a[i]*b[i];
        result.push_back(value);
    }
    return result;
}

float SVM::dot(vector<float> a, vector<float> b)
{
    int i = 0;
    float result = 0.0f;
    for(; i < a.size(); i++) {
        result += a[i]*b[i];
    }
    return result;
}

float SVM::calcError(int index)
{
    vector<float> kernelResult;
    for(int i = 0;  i < mDataSet.size(); i++) {
        kernelResult.push_back(runKernel(mDataSet[i], mDataSet[index]));
    }

    int u = dot(multiply(mAlpha, mDataLabel), kernelResult) + mBias;
    int error = u - mDataLabel[index];
    return error;
}

int SVM::selectAnotherIndex(int index, float error, float& nextError)
{
    int maxK = -1;
    float maxDeltaError = 0.0f;
    mErrorCache[index] = error;
    //find the dataset whose error is not zero
    auto iter = mErrorCache.begin();
    for(; iter != mErrorCache.end(); iter++) {
        int k = iter->first;
        if((iter->second != 0.0) && (k != index)) {
            int errorK = calcError(k);
            float deltaError = abs(errorK - error);
            if(deltaError > maxDeltaError) {
                maxK = k;
                maxDeltaError = deltaError;
                nextError = errorK;
            }
        }
    }

    if(maxK == -1) {
        //selet one dataset randomly
        maxK = index;
        while(maxK == index)
            maxK = random(mDataSet.size());

        nextError = calcError(maxK);
    }
    return maxK;
}

float SVM::clip(float value, float high, float low)
{
    if(value > high)
        value = high;
    if(value < low)
        value = low;
    return value;
}

int SVM::checkExample(int indexI)
{
    float errorI = calcError(indexI);
    float rI = errorI * mDataLabel[indexI];

    if((rI+mSlack) < 0 && (mAlpha[indexI] < mMargin) ||
       (rI > mSlack && mAlpha[indexI] >0)) {
        float errorJ = 0.0f;
        int indexJ = selectAnotherIndex(indexI, errorI, errorJ);
        float alphaI = mAlpha[indexI];
        float alphaJ = mAlpha[indexJ];

        float L = 0.0f;
        float H = 0.0f;
        if(mDataLabel[indexI] != mDataLabel[indexJ]) {
            L = max(0.0f, mAlpha[indexJ] - mAlpha[indexI]);
            H = min(mMargin, mMargin + mAlpha[indexJ] - mAlpha[indexI]);
        } else {
            L = max(0.0f, mAlpha[indexJ] + mAlpha[indexI] - mMargin);
            H = min(mMargin, mAlpha[indexJ] + mAlpha[indexI]);
        }

        if(L == H)
            return 0;

        float k11 = runKernel(mDataSet[indexI], mDataSet[indexI]);
        float k12 = runKernel(mDataSet[indexI], mDataSet[indexJ]);
        float k22 = runKernel(mDataSet[indexJ], mDataSet[indexJ]);

        float eta = 2*k12 - k11 - k22;
        if(eta >= 0)
            return 0;

        mAlpha[indexJ] -= mDataLabel[indexJ]*(errorI - errorJ)/eta;
        mAlpha[indexJ] = clip(mAlpha[indexJ], H, L);

        //update error for indexJ;
        mErrorCache[indexJ] = calcError(indexJ);

        if(abs(mAlpha[indexJ] - alphaJ) < 0.00001)
            return 0;

        mAlpha[indexI] += mDataLabel[indexJ]*mDataLabel[indexI]*(alphaJ - mAlpha[indexJ]);

        //update error for indexI;
        mErrorCache[indexI] = calcError(indexI);

        float b1 = mBias - errorI- mDataLabel[indexI]*(mAlpha[indexI]-alphaI)*k11 - mDataLabel[indexJ]*(mAlpha[indexJ]-alphaJ)*k12;
        float b2 = mBias - errorJ- mDataLabel[indexI]*(mAlpha[indexI]-alphaI)*k12- mDataLabel[indexJ]*(mAlpha[indexJ]-alphaJ)*k22;
        if((0 < mAlpha[indexI]) && (mMargin> mAlpha[indexI]))
            mBias = b1;
        else if((0 < mAlpha[indexJ]) && (mMargin > mAlpha[indexJ]))
            mBias = b2;
        else
            mBias = (b1 + b2)/2.0;
        return 1;
    }

    return 0;
}

void SVM::printSVMInfo()
{
    int svNum = 0;
    for(int i = 0; i < mDataSetCount; i++) {
        if(mAlpha[i] != 0.0)
            svNum++;
    }
    printf("there are %d support vectors\n", svNum);
}

void SVM::train()
{
    int loop = 0;
    bool examineAll = true;
    int numChanged = 0;
    //initialize each item of mAlpha to 1.0
    int i = 0;
    for(i = 0; i < mDataSetCount; i++)
        mAlpha.push_back(0.0f);

    mBias = 0.0f;

    while((loop < mMaxLoop) && (examineAll || numChanged > 0)) {
        numChanged = 0;
        if(examineAll) {
            for(i = 0; i < mDataSetCount; i++)
                numChanged += checkExample(i);
        } else {
            //loop the data whose lagrange multipliers is not zero or mMargin
            for(i = 0; i < mDataSetCount; i++) {
                if((mAlpha[i] != 0.0) && (mAlpha[i] != mMargin))
                    numChanged += checkExample(i);
            }
        }

        if(examineAll)
            examineAll = false;
        else if(numChanged == 0)
            examineAll = true;

        loop++;
        printf("loop:%d \n", loop);
    }

    printSVMInfo();
}

#define MAX_LINE 1024

void svm_test()
{
    SVM* svm = new SVM(200, 0.0001, 10000, "rbf", 20);

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
    svm->loadDataSet(dataSet);

    svm->train();

    fclose(file);

    delete svm;
}
