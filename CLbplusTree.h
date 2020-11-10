#pragma once
#include "bplusTreeDefine.h"
#include "bplusTreeNode.h"
#include "NodeDiskMemory.h"
#include <vector>
#include <fstream>
using namespace std;
struct SearchResult
{
	int keyIndex;
	STNodeDiskMemory targetNode;
	SearchResult() { keyIndex = -1;  }
};
class CLbplusTree
{
public:
	CLbplusTree();
	void nodeInsert(KeyType key, const DataType& data, int childOffset, int childIndex, STNodeDiskMemory& node);
	//void nodeInsert(KeyType key, const DataType& data, STNodeDiskMemory& node);
	~CLbplusTree() {};
	bool insert(KeyType key, const DataType &data);
	//范围查找，输入为上下界的key值和指示是否包含上下界的bool变量。
	bool select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType>& selectResult);
	bool select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult);
private:
	void nodeSplit(STNodeDiskMemory& node, KeyType& splitNewKey, int& splitNewChild);
	void adjustRoot();
	//在以node为根节点的子树中寻找key
	bool check(CLbplusTreeNode* node ,KeyType key);
	bool insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data);
	STNodeDiskMemory GetNodeFromDisk(int offset);
	void SetNodeToDisk(STNodeDiskMemory& node, int offset);
	bool search(int offset, KeyType key, SearchResult& result);
	int AssignNewNodeOffset();
private:
	fstream m_File;
	int m_NewNodeOffset;
	int m_CurrentGetOffset;
	int m_RootOffset;
	int m_DataHeadOffset;
	KeyType m_MaxKey;  // B+树中的最大键
};

