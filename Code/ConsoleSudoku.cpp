#include "ConsoleSudoku.h"

/************************************************************************
* 功能 : 默认构造。
* 参数 : 无。
* 返回 : 无。
************************************************************************/
ConsoleSudoku::ConsoleSudoku()
{
    m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

/************************************************************************
* 功能 : 重置。
* 参数 :
*        resetChar -> 当前键入的字符(判断是否是'r')。
* 返回 : 键入的字符是'r'并重置返回true，否则返回false。
************************************************************************/
bool ConsoleSudoku::Reset(signed char resetChar)
{
    if (resetChar != 'r')
    {
        return false;
    }
    m_sudoku.Reset();
    m_lastNum = -1;
    //////////////////////////////////////////////////////////////////////////
    SetConsoleCursorPosition(m_hStdOut, { SUDOKU_START_X, SUDOKU_START_Y });
    int num;
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; j++)
        {
            num = m_sudoku.GetNumber(i, j);
            if (num)
            {
                SetConsoleTextAttribute(m_hStdOut, 0x03);
                printf("%d\b", m_sudoku.GetNumber(i, j));

            }
            else
            {
                SetConsoleTextAttribute(m_hStdOut, 0x0E);
                printf("_\b");
            }
            MoveCursor(KEY_RIGHT);
        }
        MoveCursor(KEY_DOWN);
    }
    SetConsoleTextAttribute(m_hStdOut, 0x0E);
    return true;
}

/************************************************************************
* 功能 : 撤销。
* 参数 :
*        undoChar -> 当前键入的字符(判断是否是'u')。
* 返回 : 键入的字符是'u'则返回上一次输入前的字符，否则还是此次输入的字符。
* 备注 : 函数会将光标移动到上依次输入的位置。
************************************************************************/
signed char ConsoleSudoku::Undo(signed char undoChar)
{
    if (undoChar != 'u' || m_lastNum < 0)
    {
        return undoChar;
    }
    SetConsoleCursorPosition(m_hStdOut, m_lastCoord);
    return m_lastNum;
}

/************************************************************************
* 功能 : 输入数字。
* 参数 :
*        numChar -> 当前键入的字符(判断是否是'0'-'9')。
* 返回 : 键入的字符是'0'-'9'返回true，否则返回false。
************************************************************************/
bool ConsoleSudoku::InputNumber(signed char numChar)
{
    //printf("numchar : %d\n", numChar);
    if (numChar < '0' || numChar > '9')
    {
        return false;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hStdOut, &csbi);
    COORD coord = { csbi.dwCursorPosition .X - SUDOKU_START_X, csbi.dwCursorPosition.Y - SUDOKU_START_Y };

    if (coord.Y > 7)
    {
        coord.Y -= 1;
    }
    if (coord.Y > 3)
    {
        coord.Y -= 1;
    }
    coord.X /= 2;
    if (coord.X > 7)
    {
        coord.X -= 1;
    }
    if (coord.X > 3)
    {
        coord.X -= 1;
    }
    if (m_sudoku.IsFixedNumber(coord.Y, coord.X))
    {
        return true;
    }

    m_lastCoord = csbi.dwCursorPosition;
    m_lastNum = m_sudoku.GetNumber(coord.Y, coord.X) + '0';

    m_sudoku.SetNumber(coord.Y, coord.X, numChar - '0');
    if (m_sudoku.GetNumber(coord.Y, coord.X))
    {
        printf("%d\b", m_sudoku.GetNumber(coord.Y, coord.X));
    }
    else
    {
        printf("_\b");
    }
    return true;
}

/************************************************************************
* 功能 : 移动光标。
* 参数 :
*        directionChar -> 当前键入的字符(判断是否是定义的方向键)。
* 返回 : 键入的字符是定义的方向键返回true，否则返回false。
************************************************************************/
bool ConsoleSudoku::MoveCursor(signed char directionChar)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hStdOut, &csbi);
    COORD coord = csbi.dwCursorPosition;
    short inc = 0;
    //////////////////////////////////////////////////////////////////////////
    switch (directionChar)
    {
    case KEY_UP:
        inc = -1;
        coord.Y += inc;
        break;
    case KEY_DOWN:
        inc = 1;
        coord.Y += inc;
        break;
    case KEY_LEFT:
        inc = -2;
        coord.X += inc;
        break;
    case KEY_RIGHT:
        inc = 2;
        coord.X += inc;
        break;
    }
    //////////////////////////////////////////////////////////////////////////
    switch (directionChar)
    {
    case 'w':
        inc = -1;
        coord.Y += inc;
        break;
    case 's':
        inc = 1;
        coord.Y += inc;
        break;
    case 'a':
        inc = -2;
        coord.X += inc;
        break;
    case 'd':
        inc = 2;
        coord.X += inc;
        break;
    }
    //////////////////////////////////////////////////////////////////////////
    if (!inc)
    {
        return false;
    }

    if (coord.Y == 8 || coord.Y == 12)
    {
        coord.Y += inc;
    }
    else if (coord.Y > SUDOKU_END_Y)
    {
        coord.Y = SUDOKU_START_Y;
    }
    else if (coord.Y < SUDOKU_START_Y)
    {
        coord.Y = SUDOKU_END_Y;
    }
    else if(coord.X == 35 || coord.X == 43)
    {
        coord.X += inc;
    }
    else if (coord.X > SUDOKU_END_X)
    {
        coord.X = SUDOKU_START_X;
    }
    else if (coord.X < SUDOKU_START_X)
    {
        coord.X = SUDOKU_END_X;
    }
    SetConsoleCursorPosition(m_hStdOut, coord);

    return true;
}

/************************************************************************
* 功能 : 显示初始界面。
* 参数 : 无。
* 返回 : 无。
************************************************************************/
void ConsoleSudoku::Show()
{
    COORD coordScreen = { 0, 0 };
    DWORD charsWrittenCount;
    DWORD screenSize;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hStdOut, &csbi);

    screenSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(m_hStdOut, _T(' '), screenSize, coordScreen, &charsWrittenCount);
    FillConsoleOutputAttribute(m_hStdOut, 0x07, screenSize, coordScreen, &charsWrittenCount);
    SetConsoleTextAttribute(m_hStdOut, 0x07);
    //////////////////////////////////////////////////////////////////////////
    FillConsoleOutputCharacter(m_hStdOut, _T('='), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    coordScreen.Y = 2;
    FillConsoleOutputCharacter(m_hStdOut, _T('-'), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    coordScreen.Y = 17;
    FillConsoleOutputCharacter(m_hStdOut, _T('-'), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    coordScreen.Y = 19;
    FillConsoleOutputCharacter(m_hStdOut, _T('-'), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    coordScreen.Y = 21;
    FillConsoleOutputCharacter(m_hStdOut, _T('-'), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    coordScreen.Y = 23;
    FillConsoleOutputCharacter(m_hStdOut, _T('='), CONSOLE_LINE_MAX, coordScreen, &charsWrittenCount);
    //////////////////////////////////////////////////////////////////////////
    coordScreen.X = 29;
    coordScreen.Y = 1;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("Console Sudoku %s", CONSOLE_SUDOKU_VERSION);

    coordScreen.X = 35;
    coordScreen.Y = 18;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("Level :");

    coordScreen.X = 3;
    coordScreen.Y = 20;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("New:n[1-6]  Reset:r  Undo:u  Exit:ESC  Move:w s a d  Number:0-9 (0:clear)");

    coordScreen.X = 19;
    coordScreen.Y = 22;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("http://github.com/wpggles/ConsoleSudoku");
    //////////////////////////////////////////////////////////////////////////
    coordScreen.X = 25;
    coordScreen.Y = 3;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("    A B C   D E F   G H I");
    coordScreen.Y = 4;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("  -------------------------");
    coordScreen.Y = 5;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("A |       |       |       |");
    coordScreen.Y = 6;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("B |       |       |       |");
    coordScreen.Y = 7;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("C |       |       |       |");
    coordScreen.Y = 8;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("  -------------------------");
    coordScreen.Y = 9;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("D |       |       |       |");
    coordScreen.Y = 10;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("E |       |       |       |");
    coordScreen.Y = 11;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("F |       |       |       |");
    coordScreen.Y = 12;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("  -------------------------");
    coordScreen.Y = 13;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("G |       |       |       |");
    coordScreen.Y = 14;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("H |       |       |       |");
    coordScreen.Y = 15;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("I |       |       |       |");
    coordScreen.Y = 16;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
    printf("  -------------------------");
    //////////////////////////////////////////////////////////////////////////
    coordScreen.X = SUDOKU_START_X;
    coordScreen.Y = SUDOKU_START_Y;
    SetConsoleCursorPosition(m_hStdOut, coordScreen);
}

/************************************************************************
* 功能 : 新建数独。
* 参数 :
*        level -> 游戏等级。
* 返回 : 无。
************************************************************************/
void ConsoleSudoku::New(SudokuLevel level)
{
    Show();
    m_sudoku.Create((short)(((float)level / 100) * (RC_COUNT * RC_COUNT)));
    //m_sudoku.Create(1);
    m_lastNum = -1;
    //////////////////////////////////////////////////////////////////////////
    COORD coord = { LEVEL_POS_X, LEVEL_POS_Y };
    SetConsoleCursorPosition(m_hStdOut, coord);
    printf("%d", (level / 5) - 5);

    coord.X = SUDOKU_START_X;
    coord.Y = SUDOKU_START_Y;
    SetConsoleCursorPosition(m_hStdOut, coord);
    int num;
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; j++)
        {
            num = m_sudoku.GetNumber(i, j);
            if (num)
            {
                SetConsoleTextAttribute(m_hStdOut, 0x03);
                printf("%d\b", m_sudoku.GetNumber(i, j));

            }
            else
            {
                SetConsoleTextAttribute(m_hStdOut, 0x0E);
                printf("_\b");
            }
            MoveCursor(KEY_RIGHT);
        }
        MoveCursor(KEY_DOWN);
    }
    SetConsoleTextAttribute(m_hStdOut, 0x0E);
}

/************************************************************************
* 功能 : 新建游戏。
* 参数 :
*        newChar -> 当前键入的字符(判断是否是'n')。
* 返回 : 键入的字符是'n'并随后输入正确的等级返回对应等级。
************************************************************************/
ConsoleSudoku::SudokuLevel ConsoleSudoku::NewGame(signed char newChar)
{
    if (newChar == 'n')
    {
        switch (_getch())
        {
        case '1':
            return Level1;
        case '2':
            return Level2;
        case '3':
            return Level3;
        case '4':
            return Level4;
        case '5':
            return Level5;
        case '6':
            return Level6;
        }
    }
    return Level0;
}

/************************************************************************
* 功能 : 运行。
* 参数 : 无。
* 返回 : 无。
************************************************************************/
void ConsoleSudoku::Run()
{
    signed char c = 0;
    bool isFinished = false;
    SudokuLevel level = Level1;

    while (c != 0x1B)
    {
        isFinished = false;
        New(level);
        time_t useTimeSec = time(NULL);
        while ((c = _getch()) != 0x1B)//ESC
        {
            if (c < 0 || MoveCursor(c) || (!isFinished && Reset(c)))
            {
                continue;
            }
            if (Level0 != (level = NewGame(c)))
            {
                break;
            }
            c = Undo(c);
            if (!isFinished && InputNumber(c) && m_sudoku.IsFinished())
            {
                isFinished = true;
                SetConsoleTextAttribute(m_hStdOut, 0x07);
                time_t doneTimeSec = time(NULL) - useTimeSec;
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(m_hStdOut, &csbi);

                COORD coordScreen = { 3, 18 };
                SetConsoleCursorPosition(m_hStdOut, coordScreen);
                printf("You finished this puzzle!");

                coordScreen.X = 53;
                SetConsoleCursorPosition(m_hStdOut, coordScreen);
                printf("Use time:%02lld:%02lld:%02lld", doneTimeSec / 60 / 60, doneTimeSec / 60 % 60, doneTimeSec % 60);

                SetConsoleCursorPosition(m_hStdOut, csbi.dwCursorPosition);
                continue;
            }
        }

    }
    
}
