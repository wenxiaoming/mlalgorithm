#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <math.h>

using namespace std;

typedef struct _TreeNode
{
    map<string, _TreeNode*> children;
    string attribute;
}TreeNode;

enum SplitAlgorithm
{
    ID3 = 0,
    C4_5 = 1,
};

class DecisionTree
{
public:
    DecisionTree();
    ~DecisionTree();
    void loadDataSet(vector<string> dataSet);
    void loadAttribute(vector<string> atribute);
    void createTree();
    string predict(string data);
    void saveTree2Dot(string filename);
private:
    int getAttributeIndex(string currAttribute);
    string predictRecursive(TreeNode* header, vector<string> convetedData);
    void saveTree2DotRecursive(TreeNode* header, FILE *fp, int depth=0);
    void releaseTree(TreeNode* header);
    void visitTree(TreeNode* header, int depth=0);
    int getLabelNum(vector<vector<string>> dataSetVec);
    string selectMajorityLabel(vector<vector<string>> dataSetVec);
    TreeNode* createTreeRecursive(vector<vector<string>> dataSetVec, vector<string> attribute);
    float calcShannonEntropy(vector<vector<string>> dataSetVec);
    int selectBestAttribute(vector<vector<string>> dataSetVec);
    vector<string> convertDataSet(string dataStr);
    vector<vector<string>> splitDataSet(vector<vector<string>> dataSetVec, int atrrIndex, string atrrValue);
    TreeNode* mHeader;
    int mDataSetCount;
    vector<vector<string>> mDataSetVec;
    vector<string> mDataSet;
    vector<string> mAttribute;
    SplitAlgorithm mSplitAlgorithm;
};

#endif
