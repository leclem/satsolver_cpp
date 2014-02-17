#ifndef KCOLOR_H
#define KCOLOR_H
#include <iostream>
#include <map>
#include <list>
class CKColor
{
    public :
    static void translateIn(std::istream& in, std::ostream& result);
    static void translateOut(std::istream& out, std::ostream& result);
    static std::map<int, std::list<int> > connections;
    static std::string fichierGraphe;

};

#endif
