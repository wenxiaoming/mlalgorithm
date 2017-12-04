#include "fpgrowth.h"

FPGrowth::FPGrowth()
{

}

FPGrowth::~FPGrowth()
{
    //release tree
    releaseTree(mTreeHeader);
    //release fpnode
    auto iter = mFPHeader.begin();
    for(; iter != mFPHeader.end(); iter++)
    {
        //printf("delete fpnode:%s \n", (*iter)->data.c_str());
        delete (*iter);
    }
}

void FPGrowth::updateData(vector<string> data)
{
    auto iter = data.begin();
    for(; iter < data.end(); iter++)
    {
        mDataSetMap[*iter] += 1;
    }
}

//release the tree with postorder traversal
void FPGrowth::releaseTree(TreeNode* header)
{
    vector<_TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++)
        releaseTree(*iter);

    //printf("release (%s %d)\n", header->data.c_str(), header->count);
    delete header;
}

void FPGrowth::printMap(map<string, int> dataMap)
{
    printf("num string\n");
    auto iter = dataMap.begin();
    for(; iter != dataMap.end(); iter++)
    {
        printf("%d ", iter->second);
        printf("%s ", (iter->first).c_str());
        printf("\n");
    }
}

void FPGrowth::visitTree(TreeNode* header, int depth)
{
    for(int i = 0; i < depth; i++)
        printf(" ");
    printf("  (%s %d)\n", header->data.c_str(), header->count);
    vector<_TreeNode*> children = header->children;
    auto iter = children.begin();
    for(; iter != children.end(); iter++)
    {
        visitTree(*iter, depth+1);
    }
}

void FPGrowth::visitFPTree(vector<FPNode*> fpHeader)
{
    auto iter = fpHeader.begin();
    for(; iter != fpHeader.end(); iter++)
    {
        printf("%s ", (*iter)->data.c_str());
        TreeNode* nextNode = (*iter)->header;
        while(nextNode)
        {
            printf("%p  ", nextNode);
            nextNode = nextNode->next;
        }
        printf("\n");
    }
}

void FPGrowth::loadDataSet(vector<string> dataSet)
{
    for(auto iter = dataSet.begin(); iter < dataSet.end(); iter++)
    {
        vector<string> data = convertDataSet(*iter);
        mDataSetVec.push_back(data);
        mDataSet.push_back(*iter);
        updateData(data);
    }
    mDataSetCount += mDataSet.size();

    if(1)
    {
        printf("dump input dataset\n");
        printMap(mDataSetMap);
    }
}

vector<string> FPGrowth::convertDataSet(string dataStr)
{
    const char* str = dataStr.c_str();
    vector<string> retData;

    int index = 0;
    for(; index < dataStr.size(); index++)
    {
        if(isspace(*str))
            continue;
        string s;
        while(!isspace(*str))
        {
            //printf("*str:%c ", *str);
            s.insert(s.end(), *str);
            str++;
            index++;
            if(index >= dataStr.size())
                break;
        }
        retData.push_back(s);
        str++;
    }
    printf("\n");
    return retData;
}

void FPGrowth::updateFPHeader(TreeNode* node, vector<FPNode*>& fpHeader)
{
    auto iter = fpHeader.begin();
    bool findfp = false;
    FPNode* fpnode = NULL;
    for(; iter != fpHeader.end(); iter++)
    {
        FPNode* fpnode = *iter;
        if(fpnode->data.compare(node->data) == 0)
        {
            TreeNode* lastfp = fpnode->header;
            TreeNode* nextfp = lastfp;
            while(nextfp)
            {
                lastfp = nextfp;
                nextfp = lastfp->next;
            }

            lastfp->next = node;
            findfp = true;
            break;
        }
    }

    if(!findfp)
    {
        //create one new FPNode
        fpnode = new FPNode;
        fpnode->header = node;
        fpnode->data = node->data;
        fpnode->count = node->count;
        fpHeader.push_back(fpnode);
    }
}

void FPGrowth::generateTreeRecursive(TreeNode* &header, vector<FPNode*> &fpHeader, vector<PAIR> filterData)
{
    if(filterData.size() == 0)
        return;

    PAIR data = filterData[0];
    auto iter = header->children.begin();
    bool ischild = false;
    for(; iter != header->children.end(); iter++)
    {
        if((*iter)->data.compare(data.first) == 0)
        {
            ischild = true;
            break;
        }
    }

    TreeNode* child = NULL;
    if(ischild)
    {
        (*iter)->count++;
        child = *iter;
    }
    else
    {
        //create new node
        TreeNode* node = new TreeNode;
        node->count = data.second;
        node->data = data.first;
        node->parent = header;
        node->next = NULL;
        header->children.push_back(node);
        child = node;

        updateFPHeader(node, fpHeader);
    }

    if(filterData.size() > 1)
    {
        vector<PAIR> newFilterData;
        newFilterData.insert(newFilterData.begin(), filterData.begin()+1, filterData.end());
        generateTreeRecursive(child, fpHeader, newFilterData);
    }
}

void FPGrowth::removeData(map<string, int>& dataSet, int min)
{
    auto iter = dataSet.begin();
    for (; iter != dataSet.end();)
    {
        if (iter->second < min)
            dataSet.erase(iter++);
        else
            iter++;
    }
}

void FPGrowth::generateTree(TreeNode* &header, vector<FPNode*> &fpHeader,
                        vector<vector<string>> dataSetVec, map<string, int> dataSetMap, bool reset)
{
    //loop each dataset
    auto dataiter = dataSetVec.begin();
    for(; dataiter != dataSetVec.end(); dataiter++)
    {
        map<string, int> strMap;

        auto striter = (*dataiter).begin();
        for(; striter != (*dataiter).end(); striter++)
        {
            if(dataSetMap.find(*striter) != dataSetMap.end())
            {
                strMap[*striter] = dataSetMap[*striter];
            }
        }

        vector<PAIR> sortedStrMap(strMap.begin(),strMap.end());
        sort(sortedStrMap.begin(), sortedStrMap.end(), cmpbyvalue());
        if(reset)
        {
            //reset the count to 1
            for (int i = 0; i != sortedStrMap.size(); ++i)
            {
                sortedStrMap[i].second = 1;
            }
        }
        //create the tree according to the sortedStrMap with recursive method
        //and update the mFPHeader
        generateTreeRecursive(header, fpHeader, sortedStrMap);
    }
}

void FPGrowth::createTree(int min)
{
    //remove the item whose occurrence is less than min
    removeData(mDataSetMap, min);
    if(mDataSetMap.size() == 0)
        return;
    //create tree's root
    mTreeHeader = createTreeHeader();
    generateTree(mTreeHeader, mFPHeader, mDataSetVec, mDataSetMap, true);
    if(1)
    {
        printf("dump tree\n");
        visitTree(mTreeHeader);
    }
}

void FPGrowth::createConPatternBases(TreeNode* fpNode, map<vector<string>,int>& condPatternBases)
{
    TreeNode* currNode = fpNode;
    while(currNode)
    {
        vector<string> prefixPath;
        TreeNode* parentNode = currNode->parent;
        while(parentNode)
        {
             if(parentNode->data.compare("root")!= 0)
                prefixPath.push_back(parentNode->data);
            parentNode = parentNode->parent;
        }

        if(prefixPath.size() > 0)
            condPatternBases[prefixPath] = currNode->count;
        currNode = currNode->next;
    }
 }

TreeNode* FPGrowth::createTreeHeader()
{
    TreeNode* header = new TreeNode;
    header->count = 0;
    header->data = string("root");
    header->parent = NULL;
    header->next = NULL;
    return header;
}

void FPGrowth::createFPTree(map<vector<string>,int> dataSet,
                            TreeNode* &header, vector<FPNode*> &fpHeader, int min)
{
    vector<vector<string>> dataSetVec;
    map<string, int> dataSetMap;
    auto iter = dataSet.begin();
    for(; iter != dataSet.end(); iter++)
    {
        auto iter1 = (*iter).first.begin();
        for(; iter1 < (*iter).first.end(); iter1++)
        {
            dataSetMap[*iter1] += (*iter).second;
        }
        dataSetVec.push_back((*iter).first);
    }
    //remove the item whose occurrence is less than min
    removeData(dataSetMap, min);

    if(dataSetMap.size() == 0)
        return;
    //create tree's root
    header = createTreeHeader();
    generateTree(header, fpHeader, dataSetVec, dataSetMap, false);
}

void FPGrowth::mineTree(TreeNode* header, vector<FPNode*> fpHeader, int min,
                vector<string> prefix, vector<vector<string>>& freqSet, int depth)
{
    auto iter = fpHeader.begin();
    for(; iter != fpHeader.end(); iter++)
    {
        vector<string> oneFreqset;
        oneFreqset.insert(oneFreqset.begin(), prefix.begin(), prefix.end());
        oneFreqset.push_back((*iter)->data);
        freqSet.push_back(oneFreqset);
        //find condition pattern base
        map<vector<string>, int> condPatternBases;
        createConPatternBases((*iter)->header, condPatternBases);

        TreeNode* newTreeHeader = NULL;
        vector<FPNode*> newFpHeader;
        createFPTree(condPatternBases, newTreeHeader, newFpHeader, min);
        //re-create the CondPatternBases/FP tree if newFpHeader is not null
        if(newFpHeader.size() != 0)
            mineTree(newTreeHeader, newFpHeader, min, oneFreqset, freqSet, depth+1);

        //delete tree node
        if(newTreeHeader)
            releaseTree(newTreeHeader);
        //release fpnode
        auto fpiter = newFpHeader.begin();
        for(; fpiter != newFpHeader.end(); fpiter++)
        {
            delete (*fpiter);
        }
    }
}

void FPGrowth::findFrequentSet(int min, vector<vector<string>>& freqSet)
{
    vector<string> prefix;
    mineTree(mTreeHeader, mFPHeader, min, prefix, freqSet, 0);
}

void fpgrowth_test()
{
    FPGrowth* fpgrowth = new FPGrowth;

    vector<string> dataSet;
    dataSet.push_back("q r z h j p");
    dataSet.push_back("q z y x w l v u t s");
    dataSet.push_back("p z m");
    dataSet.push_back("r n l x n o s");
    dataSet.push_back("n y m r x z l q t p");
    dataSet.push_back("n y p z x e q s t m");

    fpgrowth->loadDataSet(dataSet);

    fpgrowth->createTree(3);

    vector<vector<string>> freqSet;
    fpgrowth->findFrequentSet(3, freqSet);

    printf("dump frequentset\n");
    auto iter = freqSet.begin();
    for(; iter != freqSet.end(); iter++)
    {
        auto striter = (*iter).begin();
        for(; striter != (*iter).end(); striter++)
        {
            printf("%s ", (*striter).c_str());
        }
        printf("\n");
    }

    if(fpgrowth)
        delete fpgrowth;
}