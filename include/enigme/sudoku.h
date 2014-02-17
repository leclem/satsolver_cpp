#ifndef ENIGME_SUDOKU_H
#define ENIGME_SUDOKU_H

#include <iostream>


class CSudoku
{
public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void translateOut(std::istream& out, std::ostream& result);
private:
    static int m_board[9][9];
};

#endif // ENIGME_SUDOKU_H
