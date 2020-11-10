#pragma once
#include "bplusTreeDefine.h"
using namespace std;

#pragma pack(1)
struct STNodeDiskMemory
{
	bool m_IsLeaf;
	int m_SelfOffset;
	int m_ParentOffset;
	int m_LeftSiblingOffset;
	int m_RightSiblingOffset;
	int m_KeyNum;
	KeyType m_KeyValues[UPPER_BOUND_KEYNUM + 1];
	DataType m_Datas[UPPER_BOUND_KEYNUM + 1];
	int m_ChildsOffset[UPPER_BOUND_CHILDNUM + 1];


	int getKeyIndex(KeyType key)const;
	int getChildIndex(KeyType key, int keyIndex)const;
	bool insertIsFull(KeyType key, const DataType& data, int childOffset, int childIndex);
	//void Split();
	//void insert(KeyType key, const DataType& data, fstream & file,int curOffset);
	//void split(STNodeDiskMemory node, int childIndex);
	STNodeDiskMemory getParentNodeFromDisk(fstream& file, int parentOffset);
	STNodeDiskMemory();

};
#pragma pack()
