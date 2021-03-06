#ifndef TSEITIN_H
#define TSEITIN_H
#include <iostream>
#include <map>
#include <vector>
class CTseitin
{
    public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void tseitinSub(std::stringstream& out,  std::istream& in, int level, int * variableFormula);
    static void countAndConvertVars(std::stringstream & retour, std::istream & in);
    static void translateOut(std::istream& out, std::ostream& result);
    static void syntaxModifier(std::stringstream & retour, std::istream & in);
    static int fresh();
    static int m_nbreVars;
    private:

static bool m_error;
    static std::vector<int> variablesTseitin;
    static std::map<int, std::string> m_convertion;
    static int m_nbreClauses;
    static std::map<std::string, int> m_convertionInverse;
};

#endif
