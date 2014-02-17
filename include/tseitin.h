#ifndef TSEITIN_H
#define TSEITIN_H
#include <iostream>

class CTseitin
{
    public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void tseitinSub(std::stringstream& out,  std::istream& in, int level, int * variableFormula);
    static bool m_error;
    static int fresh();
    private:
    static int m_nbreVars;
};

#endif
