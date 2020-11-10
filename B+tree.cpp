// B+tree.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CLbplusTree.h"
#include "bplusTreeDefine.h"
#include "bplusTreeNode.h"
#include <time.h>
#include <algorithm>
#include "NodeDiskMemory.h"
int main()
{
    int64_t index = 0;
    CLbplusTree tree;
    for (int i = 0; i < 10; i++)
    {
        srand((unsigned)time(NULL));

        int key = rand() % 1000;
        int data = i + 10000;
        tree.insert(i, data);
    }
    SELECT_TYPE types[5]{ SELECT_TYPE::EQ,SELECT_TYPE::GT,SELECT_TYPE::GE,SELECT_TYPE::LT,SELECT_TYPE::LE };
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 5; j++) {
            std::vector<DataType> selectResult;
            tree.select(i, types[j], selectResult);
            std::cout << "selectType:" << (int)types[j] << "### ";
            for (DataType result : selectResult) {
                std::cout << result << ",";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    struct contianPair { bool lower; bool upper; };
    contianPair pair[4] = { {true,true},{true,false},{false,true}, {false,false} };
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            std::cout << "[lower:upper]:[" << i << ":" << j << "] "<<std::endl;
            for (int k = 0; k < 4; k++)
            {
                std::vector<DataType> selectResult;           
                tree.select(i, j, pair[k].lower, pair[k].upper, selectResult);
                for (DataType result : selectResult) {
                    std::cout << result << ",";
                }
                std::cout << std::endl;
            }
            
        }
    }

    int count = sizeof(STNodeDiskMemory);
    std::cout <<count << " Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
