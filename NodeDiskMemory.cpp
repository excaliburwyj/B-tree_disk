#include "NodeDiskMemory.h"
#include <fstream>
#include "bplusTreeNode.h"
int STNodeDiskMemory::getKeyIndex(KeyType key)const
{
	int lower = 0, upper = m_KeyNum - 1;

	while (lower < upper) {
		int cmpIndex = (lower + upper) / 2;
		if (key > m_KeyValues[cmpIndex])
		{
			lower = cmpIndex + 1;
		}
		else
		{
			upper = cmpIndex;
		}
	}
	while (lower < m_KeyNum - 1 && lower < UPPER_BOUND_KEYNUM && (m_KeyValues[lower] == m_KeyValues[lower + 1]))
	{
		lower = lower + 1;
	}
	return lower;
}
int STNodeDiskMemory::getChildIndex(KeyType key, int keyIndex)const
{
	if (key >= m_KeyValues[keyIndex])
	{
		return keyIndex + 1;
	}
	else
	{
		return keyIndex;
	}

}
bool STNodeDiskMemory::insertIsFull(KeyType key, const DataType& data,int childOffset, int childIndex)
{
	if (this->m_IsLeaf)
	{
		int i;
		for (i = m_KeyNum; i >= 1 && m_KeyValues[i - 1] > key; --i)
		{
			m_KeyValues[i] = m_KeyValues[i - 1];
			m_Datas[i] = m_Datas[i - 1];
		}
		m_KeyValues[i] = key;
		m_Datas[i] = data;
		m_KeyNum += 1;

	}
	else
	{
		int i;
		for (i = m_KeyNum; i > childIndex; --i)//将父节点中的childIndex后的所有关键字的值和子树指针向后移一位
		{
			m_ChildsOffset[i + 1] = m_ChildsOffset[i];
			m_KeyValues[i] = m_KeyValues[i - 1];
		}
		if (i == childIndex + 1)
		{
			m_ChildsOffset[i + 1] = m_ChildsOffset[i];
		}
		m_ChildsOffset[childIndex + 1] = childOffset;
		m_KeyValues[childIndex] = key;
		m_KeyNum += 1;
	}
	if (m_KeyNum > UPPER_BOUND_KEYNUM)
	{
		return true;

	}
	else
	{
		return false;
	}
}
//
//void STNodeDiskMemory::Split()
//{
//
//}
//void STNodeDiskMemory::insert(KeyType key, const DataType& data, fstream  &file, int curOffset)
//{
//	if (this->m_IsLeaf)
//	{
//		int i;
//		for (i = m_KeyNum; i >= 1 && m_KeyValues[i - 1] > key; --i)
//		{
//			m_KeyValues[i] = m_KeyValues[i - 1];
//			m_Datas[i] = m_Datas[i - 1];
//		}
//		m_KeyValues[i] = key;
//		m_Datas[i] = data;
//		m_KeyNum += 1;
//
//		if (m_KeyNum > UPPER_BOUND_KEYNUM)
//		{
//			/*if (m_ParentOffset != -1)
//			{
//				STNodeDiskMemory parentNode = STNodeDiskMemory::getParentNodeFromDisk(file, m_ParentOffset);
//				int keyIndex = parentNode.getKeyIndex(key);
//				int childIndex = parentNode.getChildIndex(key, keyIndex);
//				split(parentNode, childIndex);
//			}
//			else {
//				CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
//				innerNode->setChild(0, this);
//				split(innerNode, 0);
//				setParentNode(innerNode);
//			}*/
//		}
//	}
//}

//void STNodeDiskMemory::split(STNodeDiskMemory node, int childIndex)
//{
//	if (m_IsLeaf)
//	{
//		STNodeDiskMemory newNode;//分裂后的右节点
//		newNode->setParentNode(parentNode);
//		setKeyNum(LOWER_BOUND_KEYNUM + 1);
//		newNode->setKeyNum(LOWER_BOUND_KEYNUM + 1);
//		CLbplusTreeLeafNode* rightNode = getRightSibling();
//		newNode->setRightSibling(rightNode);
//		setRightSibling(newNode);
//		newNode->setLeftSibling(this);
//		if (rightNode != NULL)
//		{
//			rightNode->setLeftSibling(newNode);
//		}
//
//		for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝关键字的值
//		{
//			newNode->setKeyValue(i, m_KeyValues[i + LOWER_BOUND_KEYNUM + 1]);
//		}
//		for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝数据
//		{
//			newNode->setData(i, m_Datas[i + LOWER_BOUND_KEYNUM + 1]);
//		}
//		((CLbplusTreeInnerNode*)parentNode)->insert(childIndex, childIndex + 1, m_KeyValues[LOWER_BOUND_KEYNUM + 1], newNode);
//	}
//}

STNodeDiskMemory STNodeDiskMemory::getParentNodeFromDisk(fstream& file, int parentOffset)
{
	file.seekg(parentOffset, ios::beg);
	char buffer[sizeof(STNodeDiskMemory)];
	file.read(buffer, sizeof(STNodeDiskMemory));
	STNodeDiskMemory node;
	memcpy(&node, buffer, sizeof(STNodeDiskMemory));
	return node;
}

STNodeDiskMemory::STNodeDiskMemory()
{
	m_IsLeaf = true;
	m_ParentOffset = -1;
	m_LeftSiblingOffset = -1;
	m_RightSiblingOffset = -1;
	m_SelfOffset = -1;
	m_KeyNum = 0;
	
	for (int i = 0; i < UPPER_BOUND_KEYNUM + 1; i++)
	{
		m_KeyValues[i] = -1;
		m_Datas[i] = -1;
		
	}
	for (int i = 0; i < UPPER_BOUND_CHILDNUM + 1; i++)
	{
		m_ChildsOffset[i] = -1;
	}
}
