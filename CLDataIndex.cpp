#include "CLDataIndex.h"

#include <time.h>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
void CLDataIndex::createIndex(std::vector<int64_t> datas, std::vector<int> rowIndexs)
{
	assert(rowIndexs.size() == datas.size());
	if (datas.size() == 0 || rowIndexs.size() != datas.size())
	{
		return;
	}
	for (int i = 0; i < rowIndexs.size(); i++)
	{
		m_BplusTree.insert(datas[i], rowIndexs[i]); 
	}
}

void CLDataIndex::createIndex(std::vector<int64_t> datePtr, std::vector<int>, std::string name)
{

}


void CLDataIndex::saveIndex(int name)
{
    char* filedata = NULL;
    std::fstream file;

    const char* path = ("../index/" + std::to_string(name)).c_str();
    file.open(path, std::ios::binary);
    
    //
    ////获取文件长度
    //std::streampos pos = ifile.tellg();
    //ifile.seekg(0, std::ios::end);
    //int size = ifile.tellg();
    //ifile.seekg(pos);

    //filedata = new char[size + 1];
    //ifile.read(filedata, size);
    //filedata[size] = 0;
    //ofile.open(name);
    //ofile << filedata;

    //delete[] filedata;
    //ofile.close();
    //ofile.clear();
    file.close();
    file.clear();
}
