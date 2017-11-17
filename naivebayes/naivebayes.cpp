#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

typedef struct
{
    int attrvalue;
    int count;
}Attribute;

class naivebayes
{
public:
    naivebayes(int attrnum);
    ~naivebayes();
    void loadPositiveSet(vector<string> trainset);
    void loadNegetiveSet(vector<string> trainset);
    void train();
    void classify(string testset);
private:
    vector<int> getAttribute(string datastring);
    void splitSet(string datastring, vector<vector<Attribute>>& attrarray);
    void printDataSet(vector<vector<Attribute>> attrarray);
    vector<string> trainSet;
    vector<int> labelSet;
    int attributeNum;
    vector<vector<Attribute>> positiveAttrArray;
    vector<vector<Attribute>> negitiveAttrArray;
    float positiveProb;//probability of positive dataset
    float negetiveProb;//probability of positive dataset
 };

naivebayes::naivebayes(int attrnum)
{
    attributeNum = attrnum;
}

naivebayes::~naivebayes()
{

}

void naivebayes::loadPositiveSet(vector<string> trainset)
{
    for(auto iter = trainset.begin(); iter < trainset.end(); iter++)
    {
        trainSet.push_back(*iter);
        labelSet.push_back(1);
    }
}

void naivebayes::loadNegetiveSet(vector<string> trainset)
{
    for(auto iter = trainset.begin(); iter < trainset.end(); iter++)
    {
        trainSet.push_back(*iter);
        labelSet.push_back(0);
    }
}

vector<int> naivebayes::getAttribute(string datastring)
{
    const char* temp = datastring.c_str();
    vector<int> attrivector;

    for(; *temp != '\0'; temp++)
    {
        //find the value of the specific attribute
         if(!isdigit(*temp))
            continue;

        int value = 0;
        while(*temp >= '0'&& *temp <= '9')
        {
            value *= 10;
            value += (*temp-'0');
            temp++;
        }

        attrivector.push_back(value);
    }

    return attrivector;
}

void naivebayes::splitSet(string datastring, vector<vector<Attribute>>& attrarray)
{
    vector<int> attrivector = getAttribute(datastring);
    auto attriter = attrarray.begin();
    bool empty = attrarray.empty();
    auto dataiter = attrivector.begin();

    for(; dataiter != attrivector.end(); dataiter++)
    {
        int value = *dataiter;

        bool found = false;
        if(!empty)
        {
            auto iter = (*attriter).begin();
            for(; iter != (*attriter).end(); iter++)
            {
                if(value == (*iter).attrvalue)
                {
                    (*iter).count++;
                    found = true;
                }
            }
        }

        if(!found)
        {
            Attribute attr;
            attr.attrvalue = value;
            attr.count = 1;
            if(empty)
            {
                vector<Attribute> attriVec;
                attriVec.push_back(attr);
                attrarray.push_back(attriVec);
            }
            else
            {
                (*attriter).push_back(attr);
            }
        }

        attriter++;
    }
}

void naivebayes::printDataSet(vector<vector<Attribute>> attrarray)
{
    auto attriter = attrarray.begin();
    int index = 0;
    for(; attriter != attrarray.end(); attriter++)
    {
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
    auto labeliter = labelSet.begin();
    for(auto iter = trainSet.begin(); iter < trainSet.end(); iter++)
    {
        if(*labeliter == 1)
        {//positive type
            splitSet(*iter, positiveAttrArray);
            positivenum++;
        }
        else
        {//negetive type
            splitSet(*iter, negitiveAttrArray);
            negitivenum++;
        }
        labeliter++;
    }

    printf("*******positive dataset************\n");
    printDataSet(positiveAttrArray);
    printf("*******negetive dataset************\n");
    printDataSet(negitiveAttrArray);

    //calculate p0 and p1
    positiveProb = (float)positivenum / (positivenum+negitivenum);
    negetiveProb = 1- positiveProb;

    printf("positiveProb:%f negetiveProb:%f \n", positiveProb, negetiveProb);
}

void naivebayes::classify(string testset)
{
    vector<int> dataiter = getAttribute(testset);
    //calculate the probability
    float p0 = positiveProb;
    float p1 = negetiveProb;
    auto iter = dataiter.begin();
    auto positiveiter = positiveAttrArray.begin();
    auto negetiveiter = negitiveAttrArray.begin();
    for(; iter != dataiter.end(); iter++)
    {
        //multiply each attribute's condition probalility
        int value = *iter;
        int count = 0;

        auto attriiter = (*positiveiter).begin();
        for(; attriiter!= (*positiveiter).end(); attriiter++)
        {
            if((*attriiter).attrvalue == value)
                count = (*attriiter).count;
        }

        p0 *= (float)count/(*positiveiter).size();
        positiveiter++;

        attriiter = (*negetiveiter).begin();
        for(; attriiter!= (*negetiveiter).end(); attriiter++)
        {
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
