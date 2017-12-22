#include "apriori.h"

Apriori::Apriori()
{
    mDataSetCount = 0;
}

Apriori::~Apriori()
{

}

void Apriori::updateData(vector<int> data)
{
    auto iter = data.begin();
    if(iter == data.end())
        return;

    for(; iter < data.end(); iter++) {
        if(find(mDataSet.begin(), mDataSet.end(), *iter)==mDataSet.end()) {
            mDataSet.push_back(*iter);
        }
    }
}

void Apriori::loadDataSet(vector<string> dataset)
{
    for(auto iter = dataset.begin(); iter < dataset.end(); iter++) {
        Attribute attribute;
        attribute.data = getAttribute(*iter);
        attribute.count = attribute.data.size();
        mAttributeVec.push_back(attribute);
        updateData(attribute.data);
    }
    mDataSetCount += dataset.size();
}

vector<int> Apriori::getAttribute(string data)
{
    const char* str = data.c_str();
    vector<int> attrVector;
    int index = 0;
    for (; index < data.size(); ) {
        //find the value of the specific attribute
        if (isspace(*str)) {
            index++;
            str++;
            continue;
        }

        int value = 0;
        while (*str >= '0'&& *str <= '9') {
            value *= 10;
            value += (*str - '0');
            str++;
            index++;
        }
        attrVector.push_back(value);
    }
    return attrVector;
}

map<vector<int>, float> Apriori::filterData(vector<vector<int>> currdata, vector<vector<int>>& mindata, float min)
{
    //find the occurence of currdata in the dataset
    //currdata is the subset of dataset
    map<vector<int>, int> filtermap;
    auto iter = mAttributeVec.begin();
    for(; iter < mAttributeVec.end(); iter++) {
        auto curriter = currdata.begin();
        for(; curriter < currdata.end(); curriter++) {
            auto searchiter = (*curriter).begin();
            bool match = true;
            for(; searchiter != (*curriter).end(); searchiter++) {
                if(find((*iter).data.begin(), (*iter).data.end(),*searchiter)
                   == (*iter).data.end()) {
                    match = false;
                    break;
                }
            }

            if(match) {
                filtermap[*curriter] += 1;
            }
        }
    }
    map<vector<int>, float> retfiltermap;
    //filter the filtermap with min
    auto itermap = filtermap.begin();
    for(; itermap != filtermap.end(); ) {
        retfiltermap[itermap->first] = itermap->second/(float)mDataSetCount;
        if(itermap->second/(float)mDataSetCount < min) {
            //remove this item
            filtermap.erase(itermap++);
        } else {
            mindata.push_back(itermap->first);
            itermap++;
        }
    }
    return retfiltermap;
}

void Apriori::printVector(vector<int> data)
{
    auto curriter = data.begin();
    for(; curriter < data.end(); curriter++)
        printf("%d ", *curriter);
}

vector<vector<int>> Apriori::generateNewSet(vector<vector<int>> mindata, int length)
{
    vector<vector<int>> newdataset;
    int datanum = mindata.size();
    for(int i = 0; i < datanum; i++) {
        for(int j = i+1; j < datanum; j++) {
            vector<int> temp = mindata[i];
            int* l1 = &(temp[0]);
            vector<int> temp1 = mindata[j];
            int* l2 = &(temp1[0]);

            if((length-2)==0 ||
            (strncmp((char*)l1, (char*)l2, (length-2)*sizeof(int))==0)) {
                temp.insert(temp.begin(), temp1.begin()+(length-2), temp1.begin()+temp1.size());
                sort(temp.begin(), temp.end());
                newdataset.push_back(temp);
            }
        }
    }
    return newdataset;
}

void Apriori::calculateFreqSet(float min)
{
    mMinSupport = min;
    sort(mDataSet.begin(), mDataSet.end());
    //filter the mDataSet with min
    vector<vector<int>> c1;
    auto iter = mDataSet.begin();
    for(; iter < mDataSet.end(); iter++) {
        vector<int> item;
        item.push_back(*iter);
        c1.push_back(item);
    }

    vector<vector<int>> l1;
    vector<vector<int>> l2;
    map<vector<int>, float> filtermap;
    filtermap = filterData(c1, l1, mMinSupport);
    mFilterMap.insert(filtermap.begin(), filtermap.end());
    mFilterDataVec.push_back(l1);
    int k = 2;
    while(l1.size() > 0) {
        l2 = generateNewSet(l1, k);
        l1.clear();
        filtermap = filterData(l2, l1, mMinSupport);
        mFilterMap.insert(filtermap.begin(), filtermap.end());
        mFilterDataVec.push_back(l1);
        k += 1;
    }
}

vector<vector<int>> Apriori::calcConf(vector<int> currdata, vector<vector<int>> freqset, float min)
{
    vector<vector<int>> pruned;
    auto iter = freqset.begin();
    for(; iter < freqset.end(); iter++) {
        vector<int> temp = currdata;
        auto iteritem = (*iter).begin();
        for(; iteritem < (*iter).end(); iteritem++) {
            temp.erase(remove(temp.begin(), temp.end(), *iteritem));
        }
        float conf = mFilterMap[currdata]/mFilterMap[temp];
        printf("conf:%f \n", conf);
        if(conf >= min) {
            //valid rules
            printf("accepted\n");
            vector<int> pruneditem;
            pruneditem.assign((*iter).begin(), (*iter).end());
            printVector(temp);
            printf("---->");
            printVector(currdata);
            printf("\n");
            pruned.push_back(pruneditem);
        }
    }
    return pruned;
}

void Apriori::generateRulesFromConf(vector<int> currdata, vector<vector<int>> freqset, float min)
{
    int index = 1;
    while(index < currdata.size()) {
        calcConf(currdata, freqset, min);
        freqset = generateNewSet(freqset, index+1);
        index++;
    }
}

void Apriori::createRules(float min)
{
    int size = mFilterDataVec.size();
    for(int i = 1; i < size; i++) {
        auto iter = mFilterDataVec[i].begin();
        for(; iter < mFilterDataVec[i].end(); iter++) {
            vector<vector<int>> freqset;
            auto iter1 = (*iter).begin();
            for(; iter1 < (*iter).end(); iter1++) {
                vector<int> h;
                h.push_back(*iter1);
                freqset.push_back(h);
            }
            if(i==1)
                calcConf(*iter, freqset, min);
            else
                generateRulesFromConf(*iter, freqset, min);
        }
    }
}

void apriori_test()
{
    Apriori* apriori = new Apriori;
    if(apriori == NULL)
        return;
    //update dataset
    vector<string> dataset;
    dataset.push_back("1 3 4");
    dataset.push_back("2 3 5");
    dataset.push_back("1 2 3 5");
    dataset.push_back("2 5");
    apriori->loadDataSet(dataset);
    apriori->calculateFreqSet(0.5);
    apriori->createRules(0.5);
    delete apriori;
}
