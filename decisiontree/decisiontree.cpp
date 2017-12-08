#include "decisiontree.h"

DecisionTree::DecisionTree()
{
    mSplitAlgorithm = C4_5;
}

DecisionTree::~DecisionTree()
{
    if(mHeader)
        releaseTree(mHeader);
}

void DecisionTree::loadAttribute(vector<string> atribute)
{
    mAttribute = atribute;
}

void DecisionTree::loadDataSet(vector<string> dataSet)
{
    for(auto iter = dataSet.begin(); iter < dataSet.end(); iter++)
    {
        vector<string> data = convertDataSet(*iter);
        mDataSetVec.push_back(data);
        mDataSet.push_back(*iter);
    }
    mDataSetCount += mDataSet.size();
}

vector<string> DecisionTree::convertDataSet(string dataStr)
{
    const char* str = dataStr.c_str();
    vector<string> retData;

    for(int index = 0; index < dataStr.size(); index++)
    {
        if(isspace(*str))
            continue;
        string s;
        while(!isspace(*str))
        {
            s.insert(s.end(), *str);
            str++;
            index++;
            if(index >= dataStr.size())
                break;
        }
        retData.push_back(s);
        str++;
    }
    return retData;
}

float DecisionTree::calcShannonEntropy(vector<vector<string>> dataSetVec)
{
    map<string, int> labelMap;
    for(int i = 0; i < dataSetVec.size(); i++)
    {
        vector<string> oneDataSet = dataSetVec[i];
        string label = oneDataSet[oneDataSet.size()-1];
        labelMap[label] += 1;
    }

    float shannonEntropy = 0.0;
    auto iter = labelMap.begin();
    for(; iter != labelMap.end(); iter++)
    {
        float prob = (float)(iter->second)/dataSetVec.size();
        shannonEntropy -= prob * log(prob)/log(2);
    }
    return shannonEntropy;
}

vector<vector<string>> DecisionTree::splitDataSet(vector<vector<string>> dataSetVec, int atrrIndex, string atrrValue)
{
    vector<vector<string>> newDataSetVec;
    auto iter = dataSetVec.begin();
    for(; iter < dataSetVec.end(); iter++)
    {
        if(atrrValue.compare((*iter)[atrrIndex]) == 0)
        {
            (*iter).erase((*iter).begin()+atrrIndex);
            newDataSetVec.push_back(*iter);
        }
    }
    return newDataSetVec;
}

int DecisionTree::selectBestAttribute(vector<vector<string>> dataSetVec)
{
    int numAttribute = dataSetVec[0].size()-1;
    float baseEntropy = calcShannonEntropy(dataSetVec);

    float bestInfoGain = 0.0;
    float bestGainRatio = 0.0;//for C4.5
    int bestAttribute = -1;

    for(int i = 0; i < numAttribute; i++)
    {
        //get the unique value for ith attribute
        map<string, int> uniqueMap;
        auto iter = dataSetVec.begin();
        for(; iter < dataSetVec.end(); iter++)
            uniqueMap[(*iter)[i]] += 1;

        float newEntropy = 0.0;
        float splitInfo = 0.0;//for C4.5

        auto itermap = uniqueMap.begin();
        for(; itermap != uniqueMap.end(); itermap++)
        {
            vector<vector<string>> newDataSetVec = splitDataSet(dataSetVec, i, itermap->first);

            float prob = newDataSetVec.size()/float(dataSetVec.size());
            newEntropy += prob * calcShannonEntropy(newDataSetVec);

            if(mSplitAlgorithm == C4_5)//for C4.5
                splitInfo -= prob*log(prob)/log(2);
        }

        float infoGain = baseEntropy - newEntropy;
        if(mSplitAlgorithm == C4_5)
        {//for C4.5
            float gainRatio = infoGain/splitInfo;
            if(gainRatio > bestGainRatio)
            {
                bestGainRatio = gainRatio;
                bestAttribute = i;
            }
        }
        else
        {//ID3
            if(infoGain > bestInfoGain)
            {
                bestInfoGain = infoGain;
                bestAttribute = i;
            }
        }
    }
    return bestAttribute;
}

void DecisionTree::createTree()
{
    vector<int> usedIndex;
    mHeader = createTreeRecursive(mDataSetVec, mAttribute);

    if(1)
    {
        visitTree(mHeader);
    }
}

int DecisionTree::getAttributeIndex(string currAttribute)
{
    int i = 0;
    auto iter = mAttribute.begin();
    for(; iter != mAttribute.end(); iter++)
    {
        if(currAttribute.compare(*iter) == 0)
            return i;
        i++;
    }
    assert(1);
    return -1;
}

string DecisionTree::predictRecursive(TreeNode* header, vector<string> convetedData)
{
    string currAttribute = header->attribute;
    map<string, _TreeNode*> children = header->children;
    //stop the recursive if no children
    if(children.empty())
        return currAttribute;

    //get value of the specific attribute.
    int index = getAttributeIndex(currAttribute);
    string attributeValue = convetedData[index];

    auto iter = children.begin();
    for(; iter != children.end(); iter++)
    {
        if((iter->second) && ((iter->first).compare(attributeValue) == 0))
            return predictRecursive(iter->second, convetedData);
    }
    return NULL;
}

string DecisionTree::predict(string data)
{
    vector<string> convetedData = convertDataSet(data);
    return predictRecursive(mHeader, convetedData);
}

//release the tree with postorder traversal
void DecisionTree::releaseTree(TreeNode* header)
{
    map<string, _TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++)
    {
        if((iter->second))
            releaseTree(iter->second);
    }
    delete header;
}

void DecisionTree::visitTree(TreeNode* header, int depth)
{
    for(int i = 0; i < depth; i++)
        printf(" ");
    printf(" (%s )\n", header->attribute.c_str());
    map<string, _TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++)
    {
        for(int i = 0; i < depth; i++)
            printf(" ");

        printf(" (%s )\n", (iter->first).c_str());
        if((iter->second))
            visitTree(iter->second, depth+1);
    }
}

int DecisionTree::getLabelNum(vector<vector<string>> dataSetVec)
{
    int labelIndex = dataSetVec[0].size() - 1;
    vector<string> uniqueLabel;
    auto iter = dataSetVec.begin();
    for(; iter != dataSetVec.end(); iter++)
    {
        uniqueLabel.push_back((*iter)[labelIndex]);
    }
    sort(uniqueLabel.begin(), uniqueLabel.end());
    uniqueLabel.erase(unique(uniqueLabel.begin(), uniqueLabel.end()), uniqueLabel.end());
    return uniqueLabel.size();
}

string DecisionTree::selectMajorityLabel(vector<vector<string>> dataSetVec)
{
    map<string, int> uniqueMap;
    auto iter = dataSetVec.begin();
    for(; iter < dataSetVec.end(); iter++)
        uniqueMap[(*iter)[0]] += 1;

    auto itermap = uniqueMap.begin();
    int majority = 0;
    string majorityStr;
    for(; itermap != uniqueMap.end(); itermap++)
    {
        if(itermap->second > majority)
        {
            majority = itermap->second;
            majorityStr = itermap->first;
        }
    }
    return majorityStr;
}

TreeNode* DecisionTree::createTreeRecursive(vector<vector<string>> dataSetVec, vector<string> attribute)
{
    if(0 == dataSetVec.size())
        return NULL;

    TreeNode* node = NULL;
    if(getLabelNum(dataSetVec) == 1)
    {
        node = new TreeNode;
        node->attribute = dataSetVec[0][dataSetVec[0].size() - 1];
        return node;
    }

    //there is no more attribute to split, only label in the dataset
    if(dataSetVec[0].size() == 1)
    {
        string majorityLabel = selectMajorityLabel(dataSetVec);
        node = new TreeNode;
        node->attribute = majorityLabel;
        return node;
    }

    //find the best attribute to split the tree
    int bestAttributeIndex = selectBestAttribute(dataSetVec);
    string bestAttributeStr = attribute[bestAttributeIndex];

    vector<string> newAttribute = attribute;
    newAttribute.erase(remove(newAttribute.begin(), newAttribute.end(), bestAttributeStr));
    //create tree's node
    node = new TreeNode;
    node->attribute = bestAttributeStr;
    //create the node's children if exist
    vector<string> uniqueValue;
    auto iter = dataSetVec.begin();
    for(; iter != dataSetVec.end(); iter++)
    {
        uniqueValue.push_back((*iter)[bestAttributeIndex]);
    }

    sort(uniqueValue.begin(), uniqueValue.end());
    uniqueValue.erase(unique(uniqueValue.begin(), uniqueValue.end()), uniqueValue.end());

    int i = 0;
    for(; i < uniqueValue.size(); i++)
    {
        node->children[uniqueValue[i]] = createTreeRecursive(splitDataSet(dataSetVec, bestAttributeIndex, uniqueValue[i]), newAttribute);
    }
    return node;
}

void DecisionTree::saveTree2DotRecursive(TreeNode* header, FILE *fp, int depth)
{
    map<string, _TreeNode*> children = header->children;
    //print node
    if (children.empty())
        fprintf(fp, "node[shape=record,style=filled,color=red,fontcolor=white];\n");
    else
        fprintf(fp, "node[shape=record,style=filled,color=black,fontcolor=white];\n");
    //print edge
    fprintf(fp, "%d[label=\"%s\"];\n", header, header->attribute.c_str());
    //loop its children
    auto iter = children.begin();
    for(; iter != children.end(); iter++)
    {
        if((iter->second))
        {
            fprintf(fp, "%d->%d [label=\"%s\"];\n", header, iter->second, (iter->first).c_str());
            saveTree2DotRecursive(iter->second, fp, depth+1);
        }
    }
}

void DecisionTree::saveTree2Dot(string filename)
{
    FILE* fp = fopen(filename.c_str(), "w+");
    fprintf(fp, "digraph G{\n");
    int depth = 0;
    saveTree2DotRecursive(mHeader, fp, depth);
    fprintf(fp, "}");
    fclose(fp);
}

void decisiontree_test()
{
    DecisionTree* decisiontree = new DecisionTree;

    vector<string> dataSet;
    dataSet.push_back("1, 1, 4, no");
    dataSet.push_back("1, 3, 5, yes");
    dataSet.push_back("3, 2, 5, yes");
    dataSet.push_back("2, 2, 5, yes");
    dataSet.push_back("3, 2, 5, yes");
    dataSet.push_back("2, 3, 4, yes");
    dataSet.push_back("2, 1, 5, no");
    dataSet.push_back("3, 2, 4, yes");
    dataSet.push_back("2, 1, 4, yes");
    dataSet.push_back("1, 1, 5, no");

    decisiontree->loadDataSet(dataSet);

    vector<string> atribute;
    atribute.push_back("logging frequency");
    atribute.push_back("friend num");
    atribute.push_back("true icon");

    decisiontree->loadAttribute(atribute);

    decisiontree->createTree();

    decisiontree->saveTree2Dot("tree.dot");

    string predictLabel = decisiontree->predict("1, 2, 4,");

    printf("the predicted label for data \"1, 2, 4,\" is: %s \n", predictLabel.c_str());

    if(decisiontree)
        delete decisiontree;
}
