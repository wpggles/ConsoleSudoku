#include "Sudoku.h"

/************************************************************************
* ���� : Ĭ�Ϲ��졣
* ���� : �ޡ�
* ���� : �ޡ�
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
* ���� : ʵ�ʴ���������ͼ�ĵݹ麯����
* ���� :
*        num   -> ��ǰ��������(��1��ʼ)��
*        count -> ��ǰ��䵽�ڼ����Ź���(��1��ʼ)��
* ���� : �ݹ鴴���ɹ�����true�����򷵻�false��
************************************************************************/
bool Sudoku::Create(int num, int count)
{
    //�������һ�����֣��������һ��
    if (count > RC_COUNT)
    {
        num += 1;
        count = 1;
    }
    //���ȫ�������ⷵ��
    if (num > RC_COUNT)
    {
        return true;
    }

    //����λ�õ�����
    int exchangePos = RC_COUNT;

    int br = ((count - 1) / RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] / RC_SQUARE;
    int bc = ((count - 1) % RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] % RC_SQUARE;

    do
    {
        //�����ǰλ���Ѿ������֣����ǻ�û�г�ѭ�������ظ����򽻻�λ�ã����¸�ֵ
        while (m_useMap[br][bc] || Isrepeated(br, bc, num))
        {
            //���λ��
            m_useMap[br][bc] = 0;
            //���û�н�����λ�ã�˵��ʧ�ܣ�����false
            if (--exchangePos <= num - 1)
            {
                return false;
            }
            //����λ��
            int tmp = m_randPos[count - 1][exchangePos];
            m_randPos[count - 1][exchangePos] = m_randPos[count - 1][num - 1];
            m_randPos[count - 1][num - 1] = tmp;

            //���¼�������
            br = ((count - 1) / RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] / RC_SQUARE;
            bc = ((count - 1) % RC_SQUARE) * RC_SQUARE + m_randPos[count - 1][num - 1] % RC_SQUARE;
        }
        
        m_useMap[br][bc] = num;

    } while (!Create(num, count + 1));

    return true;
}

/************************************************************************
* ���� : ����������Ϸ��ͼ��
* ���� :
*        hideCount -> Ϊ����Ϸȥ��������������
* ���� : �����ɹ�����true�����򷵻�false��
* ��ע : ���ȥ������������С��0���ߴ������ֵ(81)����ʧ�ܡ�
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
    
    //������������飬������������ַ���λ��
    for (int i = 0; i < RC_COUNT; ++i)
    {
        int tmpPos = 0;
        for (int j = 0; j < RC_COUNT; ++j)
        {
            tmpNum[j] = j;
        }
        for (int j = RC_COUNT; j > 0; --j)
        {
            //˳���ʼ����ͼ
            m_useMap[i][j - 1] = 0;

            tmpPos = rand() % j;
            m_randPos[i][j - 1] = tmpNum[tmpPos];
            tmpNum[tmpPos] = tmpNum[j - 1];
        }
    }
    //����������ͼ
    Create(1, 1);

    //�����ɾ����ͼ�е�����
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
* ���� : ����������
* ���� : �ޡ�
* ���� : �ޡ�
************************************************************************/
void Sudoku::Reset()
{
    m_fillCount = 0;
    for (int i = 0; i < RC_COUNT; ++i)
    {
        for (int j = 0; j < RC_COUNT; ++j)
        {
            //˳���ʼ����ͼ
            m_useMap[i][j] = m_useMap[i][j] < 0 ? 0 : m_useMap[i][j];
        }
    }
}

/************************************************************************
* ���� : �жϵ�ָ��λ�������Ƿ����ظ��Ĵ��ڡ�
* ���� :
*        row    -> ���������С�
*        col    -> ���������С�
*        tryNum -> �����������֣�
*                  Ĭ��[0]��
* ���� : �����������Ѿ�������ͬ���ַ���true�����򷵻�false��
* ��ע : 1.tryNum���Ϊ0(Ĭ��ֵ)�����ж�ָ��λ�õ������Ƿ��Ѿ��ظ������
*        ��Ϊ0����ʹ�ô����ֳ䵱��ָ���������ж��Ƿ��Ѿ��ظ���
*        2.���λ������С��0���ߴ������ֵ(9),��ͳһ����true��
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
            //0-8ͨ������Ĺ�ʽ�ɼ�����������ھŹ���λ�ã���һ�е�һ�С���һ�еڶ��С���һ�е����С��ڶ��е�һ�У��Դ����Ƶ������е�����
            int br = (row / RC_SQUARE) * RC_SQUARE + i / RC_SQUARE;
            int bc = (col / RC_SQUARE) * RC_SQUARE + i % RC_SQUARE;
            //�ж�ָ���������ڵ��С��С��Ź���(��ȥ����λ��)�Ƿ��е�ǰ���֣��еĻ�˵���ظ�������true
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
* ���� : �ж�ָ��λ�õ������Ƿ���ϵͳ���ɵĲ����޸ĵ����֡�
* ���� :
*        row -> ���������С�
*        col -> ���������С�
* ���� : �ǹ̶����ַ���true�����򷵻�false��
* ��ע : ���λ������С��0���ߴ������ֵ(9),��ͳһ����true��
************************************************************************/
bool Sudoku::IsFixedNumber(int row, int col)
{
    return (row >= 0 && row < RC_COUNT && col >= 0 && col < RC_COUNT) ? (m_useMap[row][col] > 0) : true;
}

/************************************************************************
* ���� : ��ȡָ��λ�õ����֡�
* ���� :
*        row -> ���������С�
*        col -> ���������С�
* ���� : ָ��λ�õ����֡�
* ��ע : ���λ������С��0���ߴ������ֵ(9),��ͳһ����0��
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
* ���� : �������ֵ�ָ��λ�á�
* ���� :
*        row -> ���������С�
*        col -> ���������С�
*        num -> �趨�����֡�
* ���� : ���óɹ�����true�����򷵻�false��
* ��ע : ���λ�����������֣�С��0���ߴ������ֵ(9),��ͳһ����false��
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
* ���� : �ж��Ƿ�ȫ����ȷ��ɡ�
* ���� : �ޡ�
* ���� : ȫ����ȷ��ɷ���true�����򷵻�false��
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
