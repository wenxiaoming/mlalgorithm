#include "naivebayes.h"

naivebayes::naivebayes(int num)
{
    mAttributeNum = num;
}

naivebayes::~naivebayes()
{

}

void naivebayes::loadPositiveSet(vector<string> trainSet)
{
    for(auto iter = trainSet.begin(); iter < trainSet.end(); iter++) {
        mTrainSet.push_back(*iter);
        mLabelSet.push_back(1);
    }
}

void naivebayes::loadNegetiveSet(vector<string> trainSet)
{
    for(auto iter = trainSet.begin(); iter < trainSet.end(); iter++) {
        mTrainSet.push_back(*iter);
        mLabelSet.push_back(0);
    }
}

vector<int> naivebayes::getAttribute(string dataStr)
{
    const char* str = dataStr.c_str();
    vector<int> attrVector;
    int index = 0;
    for (; index < dataStr.size(); ) {
        //find the value of the specific attribute
        if (isspace(*str)) {
            index++;
            str++;
            continue;
        }

        int value = 0;
        while(*str >= '0'&& *str <= '9') {
            value *= 10;
            value += (*str -'0');
            str++;
            index++;
        }
		attrVector.push_back(value);
    }
    return attrVector;
}

void naivebayes::splitSet(string dataStr, vector<vector<Attribute>>& attrArray)
{
    vector<int> attribute = getAttribute(dataStr);
    auto attriter = attrArray.begin();
    bool empty = attrArray.empty();
    auto dataiter = attribute.begin();

    for(; dataiter != attribute.end(); dataiter++) {
        int value = *dataiter;

        bool found = false;
        if(!empty) {
            auto iter = (*attriter).begin();
            for(; iter != (*attriter).end(); iter++) {
                if(value == (*iter).attrvalue) {
                    (*iter).count++;
                    found = true;
                }
            }
        }

        if(!found) {
            Attribute attr;
            attr.attrvalue = value;
            attr.count = 1;
            if(empty) {
                vector<Attribute> attriVec;
                attriVec.push_back(attr);
                attrArray.push_back(attriVec);
            } else {
                (*attriter).push_back(attr);
            }
        }

        if (!empty)
            attriter++;
    }
}

void naivebayes::printDataSet(vector<vector<Attribute>> attrArray)
{
    auto attriter = attrArray.begin();
    int index = 0;
    for(; attriter != attrArray.end(); attriter++) {
        auto iter = (*attriter).begin();
        printf("x%d:\n", index);
        for(; iter != (*attriter).end(); iter++)
            printf("value:%d count:%d \n", (*iter).attrvalue,  (*iter).count);
        index++;
    }
}

void naivebayes::train()
{
    int positivenum = 0;
    int negitivenum = 0;
    auto labeliter = mLabelSet.begin();
    for(auto iter = mTrainSet.begin(); iter < mTrainSet.end(); iter++) {
        if(*labeliter == 1) {
            //positive type
            splitSet(*iter, mPositiveAttrArray);
            positivenum++;
        } else {
            //negetive type
            splitSet(*iter, mNegitiveAttrArray);
            negitivenum++;
        }
        labeliter++;
    }

    printf("*******positive dataset************\n");
    printDataSet(mPositiveAttrArray);
    printf("*******negetive dataset************\n");
    printDataSet(mNegitiveAttrArray);

    //calculate p0 and p1
    mPositiveProb = (float)positivenum / (positivenum+negitivenum);
    mNegetiveProb = 1- mPositiveProb;

    printf("positiveProb:%f negetiveProb:%f \n", mPositiveProb, mNegetiveProb);
}

void naivebayes::classify(string testset)
{
    vector<int> dataiter = getAttribute(testset);
    //calculate the probability
    float p0 = mPositiveProb;
    float p1 = mNegetiveProb;
    auto iter = dataiter.begin();
    auto positiveiter = mPositiveAttrArray.begin();
    auto negetiveiter = mNegitiveAttrArray.begin();
    for(; iter != dataiter.end(); iter++) {
        //multiply each attribute's condition probalility
        int value = *iter;
        int count = 0;

        auto attriiter = (*positiveiter).begin();
        for(; attriiter!= (*positiveiter).end(); attriiter++) {
            if((*attriiter).attrvalue == value)
                count = (*attriiter).count;
        }

        p0 *= (float)count/(*positiveiter).size();
        positiveiter++;

        attriiter = (*negetiveiter).begin();
        for(; attriiter!= (*negetiveiter).end(); attriiter++) {
            if((*attriiter).attrvalue == value)
                count = (*attriiter).count;
        }

        p1 *= (float)count/(*negetiveiter).size();

        negetiveiter++;
    }

    printf("for testset:%s p0:%f p1:%f \n", testset.c_str(), p0, p1);
    if(p0 > p1)
        printf("the class type is 1\n");
    else
        printf("the class type is 0\n");
}

void naivebayes_test()
{

    /* we uses the following dataset to test the bayes classifier, each data includes five attributes(x0~x4),
    the label's value(0, 1)'s the type of that data.
    we uses the first 7 data set to train the bayes classifier, then we can use the trained classifier to check the type of the last data set.
    ********************************************************
    x0 x1 x2 x3 x4 Label
    1  2  4  5  2   0
    2  3  7  6  3   1
    2  4  5  6  8   0
    4  2  6  7  4   1
    5  3  4  7  5   1
    6  7  9  10 5   1
    5  2  7  8  4   0
    2  2  7  6  4   ?
    */

    naivebayes* bayes = new naivebayes(5);

    //init positive dataset
    vector<string> positiveset;
    positiveset.push_back("2  3  7  6  3");
    positiveset.push_back("4  2  6  7  4");
    positiveset.push_back("5  3  4  7  5");
    positiveset.push_back("6  7  9  10 5");
    bayes->loadPositiveSet(positiveset);

    //init negetive dataset
    vector<string> negetiveset;
    negetiveset.push_back("1  2  4  5  2");
    negetiveset.push_back("2  4  5  6  8");
    negetiveset.push_back("5  2  7  8  4");
    bayes->loadNegetiveSet(negetiveset);

    //train
    bayes->train();

    //classify
    bayes->classify("2  2  7  6  4");

    if(bayes)
        delete bayes;
}
