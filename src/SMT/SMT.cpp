#include "SMT/SMT.h"
#include "SMT/unionfind.h"

#include <iostream>
#include <sstream>
#include <map>

// input : as tseitin, with congruence theory :
// a = b
// f(x, y) != g(h(x), y)
// a <> b
void SMT::translateIn(std::istream& in, std::ostream& out)
{
    if(tseitinStream != "")
    {
        out << tseitinStream;
        return;
    }
    //temporary stream
    std::stringstream tmp;

    // map for eq
    std::map<CEqu, int> eq_map;

    //initialise the members
    m_vars.clear();

    tmp << "p logic ";


    char ch;
    // parse the file
    while(in.get(ch))
    {
        // begin a variable
        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')
        {

            // new aquality
            std::string eq;
            do
            {
                eq.push_back(ch);
                if(!in.get(ch))
                {
                    out << "c error: end of file before end of equality\n";
                    return;
                }
            }
            while(ch != '=' && ch != '>'); // wait for "=", "!=" or "<>"

            // then we parse the next var
            do
            {
                eq.push_back(ch);
                if(!in.get(ch))
                    break;
            }
            while((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == ' ');
            // if we have a function
            bool lastP = false;
            if(ch == '(')
            {
                int num_p = 0; // number of parenthesis
                do
                {
                    if(ch == '(')
                        num_p++;
                    if(ch == ')')
                        num_p--;
                    eq.push_back(ch);
                    if(!in.get(ch) && ch)
                    {
                        out << "c error: end of file before end of equality\n";
                        return;
                    }
                }
                while(num_p);
                lastP = true;
            }

            if(!CTheoryParser::checkSyntax(eq))
            {
                out << "c error: syntax error in " << eq << std::endl;
                return;
            }
            CEqu newEq = CTheoryParser::parse(eq);
            if(eq_map.find(newEq) == eq_map.end())
            {
                std::cerr << "nv";
                eq_map[newEq] = m_vars.size();
                tmp << "V_"<<m_vars.size();
                m_vars.push_back(newEq);
            }
            else
                tmp<<"V_"<<eq_map[newEq];
            if(!lastP)
                tmp<<ch;
        }
        else
            tmp<<ch;
    }
    tseitinStream = tmp.str();
    out << tmp.str();
}


// check if two function are congruente
static bool congru(UnionFind<CFunc>& u, CFunc f0, CFunc f1)
{
    CFunc r0 = u.Find(f0);
    CFunc r1 = u.Find(f1);

    // not the same representant
    if(r0.getName() != r1.getName())
        return false;

    // represeantants of the args should be the same
    for(int i = 0; i<r0.getArity(); i++)
        if(!congru(u, u.Find(r0.args()[i]), u.Find(r1.args()[i])))
            return false;
    return true;
}

// check the theory
// return true if we must continue the algorithm
// fill out with the added input (tseitin) if we continue, else print the assignation that lead to a solvable theory
bool SMT::check(std::istream& in_s, std::ostream& out)
{
    std::stringstream in, tmp;

    std::string parse;
    while(parse != "s")
        in_s >> parse;
    in_s >> parse;

    // check if the theory is unsatisfiable
    if(parse == "UNSATISFIABLE")
    {
        out << "Unsolvable theory\n";
        return false;
    }
    char ch;
    while(in_s.get(ch))
    {
        in.put(ch);
        tmp.put(ch);
    }

    // then we make a list of our eq for the theory
    std::vector<CEqu> theory;
    while(in.good())
    {
        parse = "";
        in >> parse;
        std::cerr<<parse;
        if(parse == "")
            break;
        bool pos = true;
        if(parse[0] == '!')
            pos = true;
        std::stringstream cnv;
        int i = 0;
        while(parse[i++] != '_');
        for(int j = i; j<(int)parse.size(); j++)
            cnv << parse[j];
        cnv >> i;
        CEqu cur = m_vars[i];
        if(!pos)
            cur = !m_vars[i];
        theory.push_back((pos)? m_vars[i] : !m_vars[i]);
    }


    // list of the positions to process
    std::list<int> process;

    // fill it with the theory
    for(int i = 0; i < (int)theory.size(); i++)
        process.push_back(i);


    // unionfind for the functions
    UnionFind<CFunc> ufind;

    //list of differences
    std::vector<std::pair<CFunc, CFunc> > diff;


    // make the reductions while the list is not empty
    while(!process.empty())
    {
        // get the first
        int cur_pos = process.front();
        process.pop_front();
        std::cerr<<theory[cur_pos] << "\n";

        // check if the args are already processed
        std::list<CFunc> args;
        theory[cur_pos].getArgs(args);
        bool ready = true;
        for(std::list<CFunc>::iterator it = args.begin(); ready && it != args.end(); it++)
        {
            if(!ufind.isIncluded(*it))
                ready = false;
        }

        // if one arg is not processed, send this calculus for later
        if(!ready)
        {
            process.push_back(cur_pos);
            continue;
        }

        // vars
        CFunc left = theory[cur_pos].left();
        CFunc right = theory[cur_pos].right();

        bool eq = theory[cur_pos].isEqual();

        //check if there is a congruence
        if(congru(ufind, left, right))
        {
            if(eq)
                continue;
            else
            {
                generateNext(tmp);
                return true;
            }
        }

        // if the var are different, add them in the different list
        if(!eq)
            diff.push_back(std::make_pair(left, right));
        else
        {
            // check if there is an incompatibility with the diff list
            for(int i = 0; i<(int)diff.size(); i++)
                if((congru(ufind, left, diff[i].first) && congru(ufind, right, diff[i].second)) ||
                   (congru(ufind, right, diff[i].first) && congru(ufind, left, diff[i].second)))
            {
                generateNext(tmp);
                return true;
            }

            // they are compatible, we can merge them
            ufind.Union(left, right);
        }
    }

    out << "Satifiable theory\n";
    translateOut(tmp, out);
    return false;
}

void SMT::translateOut(std::istream& in, std::ostream& out)
{
    std::string parse;
    while(in.good())
    {
        parse = "";
        in >> parse;
        if(parse == "")
            break;
        bool pos = true;
        if(parse[0] == '!')
            pos = false;
        std::stringstream cnv;
        int i = 0;
        while(parse[i++] != '_');
        for(int j = i; j<(int)parse.size(); j++)
            cnv << parse[j];
        cnv >> i;
        CEqu cur = m_vars[i];
        if(!pos)
            cur = !m_vars[i];
        out << cur << "\n";
    }
}

void SMT::generateNext(std::istream& in)
{
    std::stringstream out;
    out << "!(";
    std::string parse;
    bool beg = true;
    while(in.good())
    {

        in >> parse;

        if(!beg)
            out << "&";
        beg = false;
        out << parse;
    }
    out << ")\n";
    tseitinStream += std::string("\n") + out.str();
}



std::vector<CEqu> SMT::m_vars = std::vector<CEqu>();
std::string SMT::tseitinStream = "";


