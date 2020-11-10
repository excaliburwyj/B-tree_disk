#include "CLbplusTree.h"
#include <map>
#include <algorithm>
#include "NodeDiskMemory.h"
std::map<SELECT_TYPE, int> selectDispatcher;

void CLbplusTree::nodeSplit(STNodeDiskMemory &node, KeyType& splitNewKey, int& splitNewChild)
{
	if (node.m_IsLeaf)
	{
		STNodeDiskMemory newNode;//分裂后的右节点
		newNode.m_SelfOffset = AssignNewNodeOffset();
		newNode.m_ParentOffset = node.m_ParentOffset;
		newNode.m_KeyNum = LOWER_BOUND_KEYNUM + 1;
		node.m_KeyNum = LOWER_BOUND_KEYNUM + 1;

		int rightNodeOffset = node.m_RightSiblingOffset;
		newNode.m_RightSiblingOffset = rightNodeOffset;
		node.m_RightSiblingOffset = newNode.m_SelfOffset;
		newNode.m_LeftSiblingOffset = node.m_SelfOffset;
		
		if (rightNodeOffset != -1)
		{
			STNodeDiskMemory rightNode = GetNodeFromDisk(rightNodeOffset);
			rightNode.m_LeftSiblingOffset = newNode.m_SelfOffset;
			SetNodeToDisk(rightNode, rightNodeOffset);
		}

		for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝关键字的值
		{
			newNode.m_KeyValues[i] = node.m_KeyValues[i + LOWER_BOUND_KEYNUM + 1];
		}
		for (int i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝数据
		{
			newNode.m_Datas[i] = node.m_Datas[i + LOWER_BOUND_KEYNUM + 1];
		}
		SetNodeToDisk(newNode, newNode.m_SelfOffset);
		splitNewKey = node.m_KeyValues[LOWER_BOUND_KEYNUM + 1];
		splitNewChild = newNode.m_SelfOffset;
	}
	else 
	{
		STNodeDiskMemory newNode;
		newNode.m_IsLeaf = false;
		newNode.m_SelfOffset = AssignNewNodeOffset();
		newNode.m_ParentOffset = node.m_ParentOffset;
		newNode.m_KeyNum = LOWER_BOUND_KEYNUM + 1;
		node.m_KeyNum = LOWER_BOUND_KEYNUM;  //更新左子树的关键字个数

		int i;
		for (i = 0; i < LOWER_BOUND_KEYNUM + 1; ++i)// 拷贝关键字的值
		{
			newNode.m_KeyValues[i] = node.m_KeyValues[i + LOWER_BOUND_KEYNUM + 1];
		}
		for (i = 0; i < LOWER_BOUND_CHILDNUM + 1; ++i) // 拷贝孩子节点指针
		{
			newNode.m_ChildsOffset[i] = node.m_ChildsOffset[i + LOWER_BOUND_CHILDNUM];
		}
		SetNodeToDisk(newNode, newNode.m_SelfOffset);
		splitNewKey = node.m_KeyValues[LOWER_BOUND_KEYNUM];
		splitNewChild = newNode.m_SelfOffset;
	}
}

void CLbplusTree::adjustRoot()
{
	STNodeDiskMemory node = GetNodeFromDisk(m_RootOffset);
	while (node.m_ParentOffset != -1)
	{
		m_RootOffset = node.m_ParentOffset;
		node = GetNodeFromDisk(m_RootOffset);

	}

}

bool CLbplusTree::check(CLbplusTreeNode* node, KeyType key)
{
	if (!node)
	{
		return false;
	}
	int keyIndex = node->getKeyIndex(key);
	int childIndex = node->getChildIndex(key, keyIndex); // 孩子结点指针索引
	if (keyIndex < node->getKeyNum() && key == node->getKeyValue(keyIndex))
	{
		return true;
	}
	if (node->getIsLeaf()) {
		return false;
	}
	else {
		return check(((CLbplusTreeInnerNode*)node)->getChild(childIndex), key);
	}
}

CLbplusTree::CLbplusTree()
{
	m_RootOffset = -1;
	m_DataHeadOffset = -1;
	m_File.open("bplustree.txt", ios::in|ios::out);
	if (!m_File.is_open())
	{
		cout << "open failed" << endl;
	}

}

void CLbplusTree::nodeInsert(KeyType key, const DataType& data, int childOffset, int childIndex, STNodeDiskMemory &node)
{
	if (node.insertIsFull(key, data, childOffset, childIndex))
	{
		if (node.m_ParentOffset != -1)
		{
			STNodeDiskMemory parentNode = GetNodeFromDisk(node.m_ParentOffset);
			int keyIndex = parentNode.getKeyIndex(key);
			int childIndex = parentNode.getChildIndex(key, keyIndex);
			KeyType parentInsertKey;
			int parentInsertChildOffset;
			nodeSplit(node, parentInsertKey, parentInsertChildOffset);
			nodeInsert(parentInsertKey, -1, parentInsertChildOffset, childIndex, parentNode);
			//((CLbplusTreeInnerNode*)parentNode)->insert(childIndex, childIndex + 1, m_KeyValues[LOWER_BOUND_KEYNUM + 1], newNode);
			SetNodeToDisk(parentNode, parentNode.m_SelfOffset);
		}
		else {
			STNodeDiskMemory newParentNode;
			newParentNode.m_IsLeaf = false;
			newParentNode.m_SelfOffset = AssignNewNodeOffset();
			newParentNode.m_ChildsOffset[0] = node.m_SelfOffset;
			node.m_ParentOffset = newParentNode.m_SelfOffset;
			KeyType parentInsertKey;
			int parentInsertChildOffset;
			nodeSplit(node, parentInsertKey, parentInsertChildOffset);
			nodeInsert(parentInsertKey, -1, parentInsertChildOffset, 0, newParentNode);
			SetNodeToDisk(newParentNode, newParentNode.m_SelfOffset);

			/*CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
			innerNode->setChild(0, this);
			split(innerNode, 0);
			setParentNode(innerNode);*/
		}
	}
	SetNodeToDisk(node, node.m_SelfOffset);
}

bool CLbplusTree::insert(KeyType key, const DataType& data) {

	SearchResult searchResult;
	if (search(m_RootOffset, key, searchResult))
	{
		STNodeDiskMemory childNode = searchResult.targetNode;
		nodeInsert(key, data, -1, -1, childNode);
		
		if (key > m_MaxKey)  // 更新最大键值
		{
			m_MaxKey = key;
		}
	}
	else
	{
		m_RootOffset = 0;
		m_DataHeadOffset = 0 ;
		m_MaxKey = key;
		STNodeDiskMemory node;
		node.m_SelfOffset = AssignNewNodeOffset();
		nodeInsert(key, data, -1, -1, node);
	}
	adjustRoot();
	//if (m_Root != NULL)
	//{
	//	if (m_Root->getKeyNum() >= UPPER_BOUND_KEYNUM) {
	//		CLbplusTreeInnerNode* innerNode = new CLbplusTreeInnerNode();
	//		innerNode->setChild(0, m_Root);
	//		m_Root->split(innerNode, 0);   
	//		m_Root = innerNode;
	//	}
	//	if (key > m_MaxKey)  // 更新最大键值
	//	{
	//		m_MaxKey = key;
	//	}
	//}
	//else
	//{
	//	m_Root = new CLbplusTreeLeafNode();
	//	m_DataHead = (CLbplusTreeLeafNode*)m_Root;
	//	m_MaxKey = key;
	//}
	//
	//insertToNode(m_Root, key, data);
	return true;
}

bool CLbplusTree::insertToNode(CLbplusTreeNode* node, KeyType key, const DataType& data)
{
	if (node == NULL)
	{
		return false;
	}
	if (node->getIsLeaf()) 
	{
		((CLbplusTreeLeafNode*)node)->insert(key, data);
	}
	else 
	{
		CLbplusTreeInnerNode* innerNode = ((CLbplusTreeInnerNode*)node);
		int keyIndex = innerNode->getKeyIndex(key);
		int childIndex = innerNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
		CLbplusTreeNode* childNode = innerNode->getChild(childIndex);
		if (childNode->getKeyNum() >= UPPER_BOUND_KEYNUM)
		{
			childNode->split(node, childIndex);
			if (node->getKeyValue(keyIndex) <= key)   // 分割后重新确定目标子结点
			{
				childNode = innerNode->getChild(childIndex + 1);
			}
		}
		insertToNode(childNode, key, data);
	}
	return true;
}

bool CLbplusTree::select(KeyType lowerBound, KeyType upperBound, bool lowerContain, bool upperContain, std::vector<DataType> & selectResult)
{
	//if (lowerBound > upperBound)
	//{
	//	return false;
	//}
	//SearchResult beginR, finalR;
	//search(m_Root,lowerBound, beginR);
	//search(m_Root,upperBound, finalR);
	//CLbplusTreeLeafNode *beginNode = beginR.targetNode, *finalNode = finalR.targetNode;
	//int beginIndex = beginR.keyIndex, finalIndex = finalR.keyIndex;
	//if (beginR.targetNode->getKeyValue(beginR.keyIndex) < lowerBound || (beginR.targetNode->getKeyValue(beginR.keyIndex) == lowerBound && !lowerContain)) {
	//	beginIndex += 1;
	//}
	//if (finalR.targetNode->getKeyValue(finalR.keyIndex) > upperBound || (finalR.targetNode->getKeyValue(finalR.keyIndex) == upperBound && !upperContain)) {
	//	finalIndex -= 1;
	//}
	//if (beginNode == finalNode)
	//{
	//	for (int i = beginIndex; i <= finalIndex; ++i)
	//	{
	//		selectResult.push_back(beginNode->getData(i));
	//	}
	//}
	//else 
	//{
	//	for (CLbplusTreeLeafNode* pLeaf = beginNode; pLeaf != finalNode; pLeaf = pLeaf->getRightSibling())
	//	{
	//		for (int i = beginIndex; i < pLeaf->getKeyNum(); ++i)
	//		{
	//			selectResult.push_back(pLeaf->getData(i));
	//		}
	//		beginIndex = 0;
	//	}
	//	for (int i = 0; i <= finalIndex; ++i)
	//	{
	//		selectResult.push_back(finalNode->getData(i));
	//	}
	//}
	//std::sort<std::vector<DataType>::iterator>(selectResult.begin(), selectResult.end());
	return true;
}

bool CLbplusTree::select(KeyType key, SELECT_TYPE selectType, std::vector<DataType>& selectResult)
{
	//if (m_Root == NULL)
	//{
	//	return false;
	//}

	//switch (selectType)
	//{
	//case SELECT_TYPE::EQ:
	//	if (key > m_MaxKey || key < m_DataHead->getKeyValue(0))
	//	{
	//		return false;
	//	}
	//	else 
	//	{
	//		SearchResult searchResult;
	//		search(m_Root, key, searchResult);
	//		if (searchResult.targetNode->getKeyValue(searchResult.keyIndex) == key) {
	//			selectResult.push_back(searchResult.targetNode->getData(searchResult.keyIndex));
	//		}
	//	}
	//	break;
	//case SELECT_TYPE::GT:
	//case SELECT_TYPE::GE:
	//{
	//	CLbplusTreeLeafNode* beginNode = m_DataHead;
	//	int beginIndex = 0;
	//	if (key >= m_DataHead->getKeyValue(0))
	//	{
	//		SearchResult searchResult;
	//		search(m_Root, key, searchResult);
	//		beginNode = searchResult.targetNode;
	//		beginIndex = searchResult.keyIndex;
	//		if (beginNode->getKeyValue(beginIndex) < key || (beginNode->getKeyValue(beginIndex) == key && selectType == SELECT_TYPE::GT))
	//		{
	//			beginIndex += 1;
	//		}
	//	}
	//	//顺序添加查找结果
	//	for (CLbplusTreeLeafNode* pLeaf = beginNode; pLeaf != NULL; pLeaf = pLeaf->getRightSibling())
	//	{
	//		for (int i = beginIndex; i < pLeaf->getKeyNum(); ++i)
	//		{
	//			selectResult.push_back(pLeaf->getData(i));
	//		}
	//		//第二个节点开始，每个key都要添加
	//		beginIndex = 0;
	//	}
	//}
	//	break;
	//case SELECT_TYPE::LT:
	//case SELECT_TYPE::LE:
	//{
	//	CLbplusTreeLeafNode* finalNode = NULL;
	//	int finalIndex = -1;
	//	if (key <= m_MaxKey)
	//	{
	//		SearchResult searchResult;
	//		search(m_Root, key, searchResult);
	//		finalNode = searchResult.targetNode;
	//		finalIndex = searchResult.keyIndex;
	//		if (finalNode->getKeyValue(finalIndex) > key || (finalNode->getKeyValue(finalIndex) == key && selectType == SELECT_TYPE::LT))
	//		{
	//			finalIndex -= 1;
	//		}
	//	}
	//	for (CLbplusTreeLeafNode* pLeaf = m_DataHead; pLeaf != finalNode; pLeaf = pLeaf->getRightSibling())
	//	{
	//		for (int i = 0; i < pLeaf->getKeyNum(); ++i)
	//		{
	//			selectResult.push_back(pLeaf->getData(i));
	//		}
	//	}
	//	if (finalNode != NULL)
	//	{
	//		for (int i = 0; i <= finalIndex; i++)
	//		{
	//			selectResult.push_back(finalNode->getData(i));
	//		}
	//	}
	//}
	//	break;
	//default:
	//	break;
	//}
	//std::sort<std::vector<DataType>::iterator>(selectResult.begin(), selectResult.end());

	return true;
}

STNodeDiskMemory CLbplusTree::GetNodeFromDisk(int offset)
{
	
	m_File.seekg(offset, ios::beg);
	char buffer[sizeof(STNodeDiskMemory)];
	m_File.read(buffer, sizeof(STNodeDiskMemory));
	STNodeDiskMemory node;
	memcpy(&node, buffer, sizeof(STNodeDiskMemory));
	return node;
}
void CLbplusTree::SetNodeToDisk(STNodeDiskMemory& node, int offset)
{

	m_File.seekp(offset, ios::beg);
	char buffer[sizeof(STNodeDiskMemory)];
	memcpy(buffer, &node, sizeof(STNodeDiskMemory));
	m_File.write(buffer, sizeof(STNodeDiskMemory));
	m_File.flush();

}
bool CLbplusTree::search(int pNodeOffset, KeyType key, SearchResult& result)
{
	if (pNodeOffset == -1)
	{
		return false;
	}
	STNodeDiskMemory node = GetNodeFromDisk(pNodeOffset);
	
	int keyIndex = node.getKeyIndex(key);
	int childIndex = node.getChildIndex(key, keyIndex); // 孩子结点指针索引
	if (node.m_IsLeaf)
	{
		result.keyIndex = keyIndex;
		result.targetNode = node;
		return true;
	}
	else
	{
		return search(node.m_ChildsOffset[childIndex], key, result);
	}
}

int CLbplusTree::AssignNewNodeOffset()
{
	int offset = m_NewNodeOffset;
	m_NewNodeOffset += sizeof(STNodeDiskMemory);
	return offset;
}
