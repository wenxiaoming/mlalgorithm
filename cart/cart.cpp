#include "cart.h"

CART::CART(int error, int num)
{
    errorThreshold = error;
    splitThreshold = num;
}

CART::~CART()
{
    if(mHeader)
        releaseTree(mHeader);
}

void CART::loadAttribute(vector<string> atribute)
{
    mAttribute = atribute;
}

void CART::loadDataSet(vector<string> dataSet)
{
    for (auto iter = dataSet.begin(); iter < dataSet.end(); iter++) {
        vector<float> data = getAttribute(*iter);
        if (data.size() != 0) {
            mDataSet.push_back(data);
        }
    }
}

vector<float> CART::getAttribute(string dataStr)
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


void CART::splitDataSet(vector<vector<float>> &smallDataSetVec, vector<vector<float>> &bigDataSetVec,
                        vector<vector<float>> dataSetVec, int atrrIndex, float atrrValue)
{
    auto iter = dataSetVec.begin();
    for(; iter < dataSetVec.end(); iter++) {
        if((*iter)[atrrIndex] > atrrValue)
            bigDataSetVec.push_back(*iter);
        else
            smallDataSetVec.push_back(*iter);
    }
    return;
}

float CART::calcError(vector<vector<float>> dataSetVec)
{
    if (dataSetVec.size() == 0)
        return 0.0f;

    auto iter = dataSetVec.begin();
    int elemSize = dataSetVec[0].size();
    vector<float> value;
    for (; iter != dataSetVec.end(); iter++) {
        value.push_back((*iter)[elemSize - 1]);
    }

    float variable = calcVariable(value);
    return variable * dataSetVec.size();
}

float CART::calcVariable(vector<float> dataSet)
{
    auto iter = dataSet.begin();
    float total = 0.0f;
    for (; iter != dataSet.end(); iter++) {
        total += *iter;
    }

    float mean = total / dataSet.size();
    iter = dataSet.begin();
    total = 0.0f;
    for (; iter != dataSet.end(); iter++) {
        total += pow((*iter - mean), 2);
    }

    return total/ dataSet.size();
}

float CART::mean(vector<vector<float>> dataSet, int index)
{
    assert(index < dataSet[0].size());
    auto iter = dataSet.begin();
    float sum = 0.0f;
    for (; iter != dataSet.end(); iter++) {
        sum += (*iter)[index];
    }
    return sum / dataSet.size();
}

int CART::selectBestAttribute(vector<vector<float>> dataSetVec, float& bestValue)
{
    int numAttribute = dataSetVec[0].size()-1;

    int bestAttribute = -1;
    float bestError = UINT_MAX;

    float originError = calcError(dataSetVec);
    int smallDataSetSize = 0;
    int bigDataSetSize = 0;

    for(int i = 0; i < numAttribute; i++) {
        //get the unique value for ith attribute
        map<float, int> uniqueMap;
        auto iter = dataSetVec.begin();
        for(; iter < dataSetVec.end(); iter++)
            uniqueMap[(*iter)[i]] += 1;

        auto itermap = uniqueMap.begin();
        for(; itermap != uniqueMap.end(); itermap++) {
            vector<vector<float>> smallDataSetVec;
            vector<vector<float>> bigDataSetVec;

            splitDataSet(smallDataSetVec, bigDataSetVec, dataSetVec, i, itermap->first);

            float currError = calcError(smallDataSetVec) + calcError(bigDataSetVec);

            if (currError < bestError) {
                bestAttribute = i;
                bestValue = itermap->first;
                bestError = currError;

                smallDataSetSize = smallDataSetVec.size();
                bigDataSetSize = bigDataSetVec.size();
            }
        }
    }

    if(((originError - bestError) < errorThreshold) || (smallDataSetSize < splitThreshold)
       || (bigDataSetSize < splitThreshold)) {
        bestValue = mean(dataSetVec, dataSetVec[0].size()-1);
        return -1;
    }
    return bestAttribute;
}

void CART::createTree()
{
    vector<int> usedIndex;
    mHeader = createTreeRecursive(mDataSet, mAttribute);

    if(1) {
        visitTree(mHeader);
    }
}

string CART::predict(string data)
{
    return "unknown";
}

//release the tree with postorder traversal
void CART::releaseTree(TreeNode* header)
{
    map<string, _TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++) {
        if((iter->second))
            releaseTree(iter->second);
    }
    delete header;
}

void CART::visitTree(TreeNode* header, int depth)
{
    for(int i = 0; i < depth; i++)
        printf(" ");
    printf(" (%s  %f)\n", header->splitAttr.c_str(), header->splitValue);
    map<string, _TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++) {
        for(int i = 0; i < depth; i++)
            printf(" ");

        printf(" (%s )\n", (iter->first).c_str());
        if((iter->second))
            visitTree(iter->second, depth+1);
    }
}

TreeNode* CART::createTreeRecursive(vector<vector<float>> dataSetVec, vector<string> attribute)
{
    if(0 == dataSetVec.size())
        return NULL;

    TreeNode* node = NULL;
    //find the best attribute to split the tree
    float bestAttributeValue = UINT_MAX;
    int bestAttributeIndex = selectBestAttribute(dataSetVec, bestAttributeValue);

    //create tree's node
    node = new TreeNode;

    if (bestAttributeIndex == -1) {
        node->splitAttr = "leaf";
        node->splitValue = bestAttributeValue;
        return node;
    }

    string bestAttributeStr = attribute[bestAttributeIndex];

    node->splitAttr = bestAttributeStr;
    node->splitValue = bestAttributeValue;
    //create the node's children
    vector<vector<float>> smallDataSetVec;
    vector<vector<float>> bigDataSetVec;

    splitDataSet(smallDataSetVec, bigDataSetVec, dataSetVec, bestAttributeIndex, bestAttributeValue);

    node->children["left"] = createTreeRecursive(smallDataSetVec, attribute);
    node->children["right"] = createTreeRecursive(bigDataSetVec, attribute);

    return node;
}

void CART::saveTree2DotRecursive(TreeNode* header, FILE *fp, int depth)
{
    map<string, _TreeNode*> children = header->children;
    //print node
    if (children.empty())
        fprintf(fp, "node[shape=record,style=filled,color=red,fontcolor=white];\n");
    else
        fprintf(fp, "node[shape=record,style=filled,color=black,fontcolor=white];\n");
    //print edge
    fprintf(fp, "%d[label=\"%s %f\"];\n", header, header->splitAttr.c_str(), header->splitValue);
    //loop its children
    auto iter = children.begin();
    for(; iter != children.end(); iter++) {
        if((iter->second)) {
            fprintf(fp, "%d->%d [label=\"%s\"];\n", header, iter->second, (iter->first).c_str());
            saveTree2DotRecursive(iter->second, fp, depth+1);
        }
    }
}

void CART::saveTree2Dot(string filename)
{
    FILE* fp = fopen(filename.c_str(), "w+");
    fprintf(fp, "digraph G{\n");
    int depth = 0;
    saveTree2DotRecursive(mHeader, fp, depth);
    fprintf(fp, "}");
    fclose(fp);
}

#define MAX_LINE 1024
void cart_test()
{
    CART* cart = new CART(1, 4);

    vector<string> dataSet;

    FILE* file = fopen("cart_dataset.txt", "rt");

    if (file == NULL) {
        printf("fail to open cart_dataset.txt\n");
        return;
    }

    char strLine[MAX_LINE];
    while (!feof(file)) {
        memset(strLine, 0, MAX_LINE);
        fgets(strLine, MAX_LINE, file);
        string s(strLine);
        dataSet.push_back(s);
    }

    cart->loadDataSet(dataSet);

    vector<string> atribute;
    atribute.push_back("logging frequency");
    atribute.push_back("icon");

    cart->loadAttribute(atribute);

    cart->createTree();

    cart->saveTree2Dot("cart.dot");

    if (file)
        fclose(file);

    if(cart)
        delete cart;
}
