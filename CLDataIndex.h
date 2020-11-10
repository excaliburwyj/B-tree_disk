#pragma once
#include <iostream>
#include "CLbplusTree.h"
#include "bplusTreeDefine.h"
#include "bplusTreeNode.h"
#include <vector>
#include <string>
class CLDataIndex
{
public:
	CLDataIndex() {
	};
	~CLDataIndex() {};
	void createIndex(std::vector<int64_t> datePtr, std::vector<int> row);
	void createIndex(std::vector<int64_t> datePtr, std::vector<int>, std::string name);
	void saveIndex(int name);
private:
	CLbplusTree m_BplusTree;
	int m_ColumnIndex;
};

