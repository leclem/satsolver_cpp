#ifndef SMT_SMT_H
#define SMT_SMT_H

#include "SMT/theory.h"
#include <sstream>

class SMT
{
public:
    static void translateIn(std::istream& in, std::ostream& out);
    static bool check(std::istream& in, std::ostream& out);

    static std::string tseitinStream;
private:
    static std::vector<CEqu> m_vars;
    static void translateOut(std::istream& in, std::ostream& out);
    static void generateNext(std::istream& in);

};

#endif // SMT_SMT_H
