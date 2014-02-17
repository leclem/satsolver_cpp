#include "enigme/latin.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "tseitin/tseitin.h"

// format:
// N M (square size; number of imposed squares)
// then M lines:
// x y v (avec x, y, v de 0 à N-1) : square[x][y] = v.
// ...


void CLatin::translateIn(std::istream& in, std::ostream& out)
{
    int N, M;
    in >> N >> M;
    m_n = N;

    /*std::ofstream tst("tst");
    out.rdbuf(tst.rdbuf());*/

    m_board = new int* [N];
    for(int i = 0; i<N; i++)
        m_board[i] = new int [N];

    for(int i = 0; i< N; i++)
        for(int j = 0; j<N; j++)
            m_board[i][j] = -1;

    // out header
    out << "p logic ";

    // variables : V_i_j_k -> square i j = k
    for(int i = 0; i<M; i++)
    {
        int a, b, v;
        in >> a  >> b >> v;
        m_board[a][b] = v;

        // we impose the values
        for(int vv = 0; vv<N; vv++)
        {
            if(vv != v)
                out << "!";
            out << "V_" << a << '_'<< b <<'_' << vv << std::endl;
        }
    }

    // only one number should be in each case
    // i, j : pos
    // k = number true
    // l = all numbers
    for(int i = 0; i< N; i++)
        for(int j = 0; j<N; j++)
        {
            bool b0 = true;
            for(int k = 0; k<N; k++)
            {
                if (!b0)
                    out << ") | (";
                else
                    out << "(";
                b0 = false;
                bool b1 = true;;
                for(int l = 0; l<N; l++)
                {
                    if(!b1)
                        out << " & ";
                    b1 = false;
                    if(k != l)
                        out << "!";
                    out << "V_" << i << '_'<< j <<'_' << l;
                }
            }
            out << ")\n";
        }

    // each two cases in a row should be different
    for(int i = 0; i<N; i++)
        for(int c1 = 0; c1<N; c1++)
            for(int c2 = c1+1; c2<N; c2++)
                for(int v = 0; v<N; v++)
                    {
                        out << "( ";
                        out << "V_" << i << '_'<< c1 <<'_' << v;
                        out << " | ";
                        out << "V_" << i << '_'<< c2 <<'_' << v;
                        out << " ) & ( !";
                        out << "V_" << i << '_'<< c1 <<'_' << v;
                        out << " | !";
                        out << "V_" << i << '_'<< c2 <<'_' << v;
                        out << " )\n";

                    }
    // each two cases in a column should be different
    for(int c = 0; c<N; c++)
        for(int l1 = 0; l1<N; l1++)
            for(int l2 = l1+1; l2<N; l2++)
                for(int v = 0; v<N; v++)
                    {
                        out << "( ";
                        out << "V_" << l1 << '_'<< c <<'_' << v;
                        out << " | ";
                        out << "V_" << l2 << '_'<< c <<'_' << v;
                        out << " ) & ( !";
                        out << "V_" << l1 << '_'<< c <<'_' << v;
                        out << " | !";
                        out << "V_" << l2 << '_'<< c <<'_' << v;
                        out << " )\n";

                    }
}

static int log10(int n)
{
    int r = 0;
    while(n)
    {
        n/= 10;
        r++;
    }
    return r;
}

void CLatin::deleteSqr()
{
    for(int i = 0; i<m_n; i++)
        delete [] m_board[i];
    delete [] m_board;
}

// format the output
static std::string format(int in, int sz)
{
    // result imposed ? set bracets
    bool br = in>=0;
    // else give the right value
    if(in<0)
        in = -in-2;
    // size of the entry
    int csz = log10(in);
    if(br)
        csz +=2;
    // output
    std::string r;
    // put spaces to have the good size
    for(int i = 0; i<(sz-csz)/2; i++)
        r.push_back(' ');
    if(br)
        r.push_back('[');
    // convert the number to a string
    std::stringstream cnv;
    cnv << in;
    std::string tmp;
    cnv >> tmp;
    r.append(tmp);
    // finish the string
    if(br)
        r.push_back(']');
    for(int i = 0; (int)r.size() < sz; i++)
        r.push_back(' ');
    return r;
}

void CLatin::translateOut(std::istream& out, std::ostream& result)
{
    std::string parse;
    while(parse != "s")
        out >> parse;
    out >> parse;

    // check if the given square is solvable
    if(parse == "UNSATISFIABLE")
    {
        result << "Unsolvable latin square\n";
        return;
    }

    // parse the result and store them into the table
    while(out.good())
    {
        out >> parse;
        // if we have a positive assignation
        if (parse[0] != '!')
        {
            int r, c, v;
            std::string nm;
            std::stringstream cnv;
            for(int cur = 0; cur < (int)parse.size(); cur++)
                if(parse[cur] == '_')
                    cnv << ' ';
                else
                    cnv << parse[cur];

            cnv >> nm >> r >> c >> v;
            if(m_board[r][c] == -1)
                m_board[r][c] = -v-2; // less than -2 to remeber it was not imposed
        }
    }


    // now we can draw the result
    int sz = log10(m_n) + 2; // size of each square
    for(int i = 0; i<m_n; i++)
    {
        for(int j = 0; j<m_n; j++)
            result << format(m_board[i][j], sz);
        result << "\n";
    }
    deleteSqr();
}

int CLatin::m_n = 0;

int** CLatin::m_board = 0;
