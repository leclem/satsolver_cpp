#include "arith/arith.h"

#include <sstream>
#include <list>
#include <vector>


// format
// N : number of lines
// on each line, an expression to eval
void CArith::translateIn(std::istream& in, std::ostream& out)
{
    int N;
    in >> N;

    num_eq = N;

    out << "p logic ";

    for(int i = 0; i<N; i++)
    {
        // we begin by separing each constant
        std::string curLine;
        std::stringstream ss;
        std::getline(in, curLine);
        for(size_t c = 0; c<curLine.size(); c++)
        {
            if(curLine[c] == '+')
                ss << ' ';
            else if(curLine[c] == '-')
                ss << " -";
            else
                ss << curLine[c];
        }
        // then we set the vars and push the var name into the sum list
        std::list<std::string> sumList;
        int cur, cnt = 0;
        unsigned transf;
        while(ss>>cur)
        {
            if(cur>0)
                transf = cur;
            else
            {
                transf = !(unsigned)(-cur) +1;
            }
            for(int c = 0; c<32; c++)
            {
                std::stringstream varName;
                varName << "Cst_"<<i<<"_"<<cnt << "_";
                if(!(transf%2))
                    out << "!";
                out << varName.str() <<c<<std::endl;
                transf/=2;
                sumList.push_back(varName.str());
            }
        }

        cnt = 0;
        // now we handle the sum
        while(sumList.size()>1)
        {
            std::stringstream varName, ret;
            // name of the variable
            if(sumList.size() == 2)
                varName << "R_" << i <<"_";
            else
                varName << "T_" << i << "_" << cnt << "_";
            // carry
            ret << "C_"<< i << "_"<<cnt << "_";

            // get the two vars to add
            std::string v0 = sumList.front(); sumList.pop_front();
            std::string v1 = sumList.front(); sumList.pop_front();

            // first bit
            // carry
            out << ret.str() << "0 & " << v0 << "0 & " << v1 <<"0\n";
            // sum
            out << varName.str() << "0 & ("<< v0 << "0 xor " << v1 <<"0)\n";
            for(int k = 1; k<32; k++)
            {
                // carry
                out << ret.str() << k << " & ((" << v0 << k << " & " << v1 << k << ") | (" << v0 << k << " & " << ret.str() << k <<") | (";
                out << ret.str() << k << " & " << v1 << k << "))\n";
                // sum
                out << varName.str() << k << " & (" << v0 << k << " xor " << v1 << k << " xor " << ret.str() << k << ")\n";
            }

            // push back the new var to make a tree
            sumList.push_back(varName.str());

            // ask for a new varName
            cnt++;
        }
    }
}

void CArith::translateOut(std::istream& in, std::ostream& out)
{
    std::vector<int> result(num_eq);
    std::string parse;
    while(parse != "s")
        in >> parse;
    in >> parse;

    // check if the given square is solvable
    if(parse == "UNSATISFIABLE")
    {
        out << "Error : unsolvable ...\n";
        return;
    }

    // parse the result and store them into the table
    while(in.good())
    {
        while(parse != "v")
            in >> parse;
        in >> parse;
        // find if we have a result
        if(parse.find('R') == std::string::npos)
            continue;

        // if we have a positive assignation
        if (parse[0] != '!')
        {
            std::string nm;
            int c, k;
            std::stringstream cnv;
            for(int cur = 0; cur < (int)parse.size(); cur++)
                if(parse[cur] == '_')
                    cnv << ' ';
                else
                    cnv << parse[cur];

            cnv >> nm >> c >> k;
            result[c] |= (1 << k);
        }
    }
    for(int i = 0; i<num_eq; i++)
        out << result[i] << std::endl;
}

int CArith::num_eq = 0;
