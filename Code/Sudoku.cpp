#include "Sudoku.h"

/************************************************************************
* 功能 : 默认构造。
* 参数 : 无。
* 返回 : 无。
************************************************************************/
Sudoku::Sudoku()
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < RC_COUNT; ++i)
    {
        for (int j = 0; j < RC_COUNT; ++j)
        {
            m_useMap[i][j] = 0;
        }
    }
}

/************************************************************************
* 功能 : 实际创建数独地图的递归函数。
* 参数 :
*        num   -> 当前填充的数字(从1开始)。
*        count -> 当前填充到第几个九宫格(从1开始)。
* 返回 : 递归创建成功返回true，否则返回false。
************************************************************************/
bool Sudoku::Create(int num, int count)
{
    //如果填完一个数字，则填充下一个
    if (count > RC_COUNT)
    {
        num += 1;
        count = 1;
    }
    //如果全部填完这返回
    if (num > RC_COUNT)
    {
        return true;
    }

    //交换位置的索引
    int exchangePos = RC_COUNT;

    int br = ((count - 1) / RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] / RC_SQUARE;
    int bc = ((count - 1) % RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] % RC_SQUARE;

    do
    {
        //如果当前位置已经有数字，但是还没有出循环或者重复，则交换位置，重新赋值
        while (m_useMap[br][bc] || Isrepeated(br, bc, num))
        {
            //清空位置
            m_useMap[br][bc] = 0;
            //如果没有交换的位置，说明失败，返回false
            if (--exchangePos <= num - 1)
            {
                return false;
            }
            //交换位置
            int tmp = m_randPos[count - 1][exchangePos];
            m_randPos[count - 1][exchangePos] = m_randPos[count - 1][num - 1];
            m_randPos[count - 1][num - 1] = tmp;

            //重新计算坐标
            br = ((count - 1) / RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] / RC_SQUARE;
            bc = ((count - 1) % RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] % RC_SQUARE;
        }
        
        m_useMap[br][bc] = num;

    } while (!Create(num, count + 1));

    return true;
}

/************************************************************************
* 功能 : 创建数独游戏地图。
* 参数 :
*        hideCount -> 为了游戏去掉的数字数量。
* 返回 : 创建成功返回true，否则返回false。
* 备注 : 如果去掉的数字数量小于0或者大于最大值(81)，则失败。
************************************************************************/
bool Sudoku::Create(short hideCount)
{
    if (hideCount < 0 || hideCount > RC_COUNT * RC_COUNT)
    {
        return false;
    }
    m_fillCount = 0;
    m_hideCount = hideCount;

    unsigned char tmpNum[RC_COUNT];
    
    //构建随机化数组，用于随机化数字放置位置
    for (int i = 0; i < RC_COUNT; ++i)
    {
        int tmpPos = 0;
        for (int j = 0; j < RC_COUNT; ++j)
        {
            tmpNum[j] = j;
        }
        for (int j = RC_COUNT; j > 0; --j)
        {
            //顺便初始化地图
            m_useMap[i][j - 1] = 0;

            tmpPos = rand() % j;
            m_randPos[i][j - 1] = tmpNum[tmpPos];
            tmpNum[tmpPos] = tmpNum[j - 1];
        }
    }
    //创建数独地图
    Create(1, 1);

    //随机化删除地图中的数字
    unsigned char tmpPosArr[RC_COUNT * RC_COUNT];
    int currentMax = RC_COUNT * RC_COUNT;
    for (int i = 0; i < RC_COUNT * RC_COUNT; ++i)
    {
        tmpPosArr[i] = i;
    }

    for (int i = 0; i < hideCount; ++i)
    {
        int index = rand() % currentMax;
        int pos = tmpPosArr[index];
        tmpPosArr[index] = tmpPosArr[--currentMax];
        m_useMap[pos / RC_COUNT][pos % RC_COUNT] = 0;
    }
    return true;
}

/************************************************************************
* 功能 : 重置数独。
* 参数 : 无。
* 返回 : 无。
************************************************************************/
void Sudoku::Reset()
{
    m_fillCount = 0;
    for (int i = 0; i < RC_COUNT; ++i)
    {
        for (int j = 0; j < RC_COUNT; ++j)
        {
            //顺便初始化地图
            m_useMap[i][j] = m_useMap[i][j] < 0 ? 0 : m_useMap[i][j];
        }
    }
}

/************************************************************************
* 功能 : 判断当指定位置数字是否有重复的存在。
* 参数 :
*        row    -> 数字所在行。
*        col    -> 数字所在列。
*        tryNum -> 尝试填充的数字，
*                  默认[0]。
* 返回 : 数独规则下已经存在相同数字返回true，否则返回false。
* 备注 : 1.tryNum如果为0(默认值)，则判断指定位置的数字是否已经重复，如果
*        不为0，则使用此数字充当当指定置数字判断是否已经重复。
*        2.如果位置索引小于0或者大于最大值(9),则统一返回true。
************************************************************************/
bool Sudoku::Isrepeated(int row, int col, int tryNum /*= 0*/)
{
    if (row < 0 || row >= RC_COUNT || col < 0 || col >= RC_COUNT)
    {
        return true;
    }

    int num = tryNum ? tryNum : m_useMap[row][col];
    if (num)
    {
        for (int i = 0; i < RC_COUNT; ++i)
        {
            //0-8通过下面的公式可计算出坐标所在九宫格位置，第一行第一列、第一行第二列、第一行第三列、第二行第一列，以此类推到第三行第三列
            int br = (row / RC_SQUARE) * RC_SQUARE + i / RC_SQUARE;
            int bc = (col / RC_SQUARE) * RC_SQUARE + i % RC_SQUARE;
            //判断指定坐标所在的行、列、九宫格(除去自身位置)是否有当前数字，有的话说明重复，返回true
            if (((num == m_useMap[row][i] || num == -m_useMap[row][i]) && i != col) || 
                ((num == m_useMap[i][col] || num == -m_useMap[i][col]) && i != row) || 
                ((num == m_useMap[br][bc] || num == -m_useMap[br][bc]) && br != row && bc != col))
            {
                return true;
            }
        }
    }
    return false;
}

/************************************************************************
* 功能 : 判断指定位置的数字是否是系统生成的不可修改的数字。
* 参数 :
*        row -> 数字所在行。
*        col -> 数字所在列。
* 返回 : 是固定数字返回true，否则返回false。
* 备注 : 如果位置索引小于0或者大于最大值(9),则统一返回true。
************************************************************************/
bool Sudoku::IsFixedNumber(int row, int col)
{
    return (row >= 0 && row < RC_COUNT && col >= 0 && col < RC_COUNT) ? (m_useMap[row][col] > 0) : true;
}

/************************************************************************
* 功能 : 获取指定位置的数字。
* 参数 :
*        row -> 数字所在行。
*        col -> 数字所在列。
* 返回 : 指定位置的数字。
* 备注 : 如果位置索引小于0或者大于最大值(9),则统一返回0。
************************************************************************/
int Sudoku::GetNumber(int row, int col)
{
    if (row < 0 || row >= RC_COUNT && col < 0 && col >= RC_COUNT)
    {
        return 0;
    }
    return m_useMap[row][col] >= 0 ? m_useMap[row][col] : -m_useMap[row][col];
}

/************************************************************************
* 功能 : 设置数字到指定位置。
* 参数 :
*        row -> 数字所在行。
*        col -> 数字所在列。
*        num -> 设定的数字。
* 返回 : 设置成功返回true，否则返回false。
* 备注 : 如果位置索引和数字，小于0或者大于最大值(9),则统一返回false。
************************************************************************/
bool Sudoku::SetNumber(int row, int col, int num)
{
    if (row >= 0 && row < RC_COUNT && col >= 0 && col < RC_COUNT && num >= 0 && num <= RC_COUNT && !IsFixedNumber(row, col))
    {
        if (m_useMap[row][col])
        {
            m_fillCount -= (num ? 0 : 1);
        }
        else
        {
            m_fillCount += (num ? 1 : 0);
        }
        m_useMap[row][col] = -num;
        
        return true;
    }
    return false;
}

/************************************************************************
* 功能 : 判断是否全部正确完成。
* 参数 : 无。
* 返回 : 全部正确完成返回true，否则返回false。
************************************************************************/
bool Sudoku::IsFinished()
{
    if (m_fillCount < m_hideCount)
    {
        return false;
    }
    for (int i = 0; i < RC_COUNT; ++i)
    {
        for (int j = 0; j < RC_COUNT; ++j)
        {
            if (!IsFixedNumber(i, j) && Isrepeated(i, j))
            {
                return false;
            }
        }
    }
    return true;
}
