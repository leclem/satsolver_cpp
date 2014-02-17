#ifndef ENIGME_MAGIC_H
#define ENIGME_MAGIC_H

#include <iostream>

class CMagic
{
public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void translateOut(std::istream& out, std::ostream& result);
private:

    static void deleteSqr();

    static int m_n;
    static int** m_board;
};

#endif // ENIGME_MAGIC_H
