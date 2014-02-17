#ifndef ENIGME_LATIN_H
#define ENIGME_LATIN_H

#include <iostream>

class CLatin
{
public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void translateOut(std::istream& out, std::ostream& result);
private:

    static void deleteSqr();

    static int m_n;
    static int** m_board;
};

#endif // ENIGME_LATIN_H
