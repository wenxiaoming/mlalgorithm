#ifndef CART_H
#define CART_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <limits.h>
#include <map>
#include <algorithm>
#include <math.h>

using namespace std;

typedef struct _TreeNode {
    map<string, _TreeNode*> children;
    string splitAttr;
    float splitValue;
} TreeNode;

class CART
{
public:
    CART(int error, int num);
    ~CART();
    void loadDataSet(vector<string> dataSet);
    void loadAttribute(vector<string> atribute);
    void createTree();
    void pruneTree();
    string predict(string data);
    void saveTree2Dot(string filename);
private:
    int getAttrIndex(string name);
    float squareSum(vector<vector<float>> dataSetVec, float delta);
    TreeNode* pruneTreeRecursive(TreeNode* header, vector<vector<float>> dataSetVec, int depth=0);
    float mean(vector<vector<float>> dataSet, int index);
    float calcError(vector<vector<float>> dataSetVec);
    float calcVariable(vector<float> dataSet);
    void saveTree2DotRecursive(TreeNode* header, FILE *fp, int depth=0);
    void releaseTree(TreeNode* header);
    void visitTree(TreeNode* header, int depth=0);
    TreeNode* createTreeRecursive(vector<vector<float>> dataSetVec, vector<string> attribute);
    int selectBestAttribute(vector<vector<float>> dataSetVec, float& bestValue);
    vector<float> getAttribute(string dataStr);
    void splitDataSet(vector<vector<float>> &smallDataSetVec, vector<vector<float>> &bigDataSetVec, vector<vector<float>> dataSetVec, int atrrIndex, float atrrValue);
    TreeNode* mHeader;
    vector<vector<float>> mDataSet;
    vector<string> mAttribute;
    int errorThreshold;
    int splitThreshold;
};

#endif
