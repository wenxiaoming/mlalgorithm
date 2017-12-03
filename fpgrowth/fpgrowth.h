#ifndef FPGROWTH_H
#define FPGROWTH_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

typedef struct _TreeNode {
    vector<_TreeNode*> children;
    _TreeNode* parent;
    _TreeNode* next;
    string data;
    int count;
} TreeNode;

typedef struct _FPNode {
    TreeNode* header;
    string data;
    int count;
} FPNode;

typedef pair<string, int> PAIR;
struct cmpbyvalue
{
    bool operator()(const PAIR& lhs, const PAIR& rhs)
    {
        return lhs.second > rhs.second;
    }
};

ostream& operator<<(ostream& out, const PAIR& p)
{
    return out << p.first << "\t" << p.second;
}

class FPGrowth
{
public:
    FPGrowth();
    ~FPGrowth();
    void loadDataSet(vector<string> dataSet);
    void createTree(int min);
    void findFrequentSet(int min, vector<vector<string>>& freqSet);
private:
    void printMap(map<string, int> dataMap);
    void visitTree(TreeNode* header, int depth = 1);
    void visitFPTree(vector<FPNode*> fpHeader);
private:
    void removeData(map<string, int>& dataSet, int min);
    TreeNode* createTreeHeader();
    void mineTree(TreeNode* header, vector<FPNode*> fpHeader, int min,
                    vector<string> prefix, vector<vector<string>>& freqSet, int depth);
    void createConPatternBases(TreeNode* fpNode, map<vector<string>,int>& condPatternBases);
    void createFPTree(map<vector<string>,int> dataSet, TreeNode* &header, vector<FPNode*> &fpHeader, int min);
    void releaseTree(TreeNode* header);
    void updateFPHeader(TreeNode* node, vector<FPNode*>& fpHeader);
    void generateTreeRecursive(TreeNode* &header, vector<FPNode*> &fpHeader, vector<PAIR> filterData);
    void updateData(vector<string> data);
    vector<string> convertDataSet(string dataStr);
    TreeNode* mTreeHeader;
    vector<FPNode*> mFPHeader;
    int mDataSetCount;
    vector<vector<string>> mDataSetVec;
    map<string, int> mDataSetMap;
    vector<string> mDataSet;
};

#endif