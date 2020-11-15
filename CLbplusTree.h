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
	bool hasIndex();
	void nodeInsert(KeyType key, const DataType& data, int childOffset, int childIndex, STNodeDiskMemory& node);
	//void nodeInsert(KeyType key, const DataType& data, STNodeDiskMemory& node);
	~CLbplusTree() {};
	bool insert(KeyType key, const DataType &data);
	//��Χ���ң�����Ϊ���½��keyֵ��ָʾ�Ƿ�������½��bool������
	bool select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType>& selectResult);
	bool select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult);
private:
	void nodeSplit(STNodeDiskMemory& node, KeyType& splitNewKey, int& splitNewChild);
	void adjustRoot();
	//����nodeΪ���ڵ��������Ѱ��key
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
	KeyType m_MaxKey;  // B+���е�����
};

