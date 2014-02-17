#include "enigme/sudoku.h"
#include <sstream>

// format:
// M (number of imposed squares)
// then M lines:
// x y v (avec x, y, v de 0 à N-1) : square[x][y] = v.
// ...

// bijection between a square number and a case number and its postion
static void convertSqr(int sq, int c, int& x, int& y)
{
    x = 3 * (sq/3) + c/3;
    y = 3 * (sq%3) + c%3;
}

void CSudoku::translateIn(std::istream& in, std::ostream& out)
{
    int M;
    in >> M;

    for(int i = 0; i<9; i++)
        for(int j = 0; j<9; j++)
            m_board[i][j] = -1;

    // out header
    out << "p logic ";

    // variables : V_i_j_k -> square i j = k
    for(int i = 0; i<M; i++)
    {
        int a, b, v;
        in >> a  >> b >> v;
        v-=1;
        m_board[a][b] = v;

        // we impose the values
        for(int vv = 0; vv<9; vv++)
        {
            if(vv != v)
                out << "!";
            out << "V_" << a << '_'<< b <<'_' << vv << std::endl;
        }
    }

    // only one number should be in each case
    for(int i = 0; i<9; i++)
        for(int j = 0; j<9; j++)
        {
            bool b0 = true;
            for(int k = 0; k<9; k++)
            {
                if (!b0)
                    out << ") | (";
                else
                    out << "(";
                b0 = false;
                bool b1 = true;;
                for(int l = 0; l<9; l++)
                {
                    if(!b1)
                        out << " & ";
                    b1 = false;
                    if(k != l)
                        out << "!";
                    out << "V_" << i << '_'<< j <<'_' << k;
                }
            }
            out << ")\n";
        }

    // each two cases in a row should be different
    for(int i = 0; i<9; i++)
        for(int c1 = 0; c1<9; c1++)
            for(int c2 = c1+1; c2<9; c2++)
                for(int v = 0; v<9; v++)
                    {
                        out << "( ";
                        out << "V_" << i << '_'<< c1 <<'_' << v;
                        out << " & !";
                        out << "V_" << i << '_'<< c2 <<'_' << v;
                        out << " ) | ( !";
                        out << "V_" << i << '_'<< c1 <<'_' << v;
                        out << " & ";
                        out << "V_" << i << '_'<< c2 <<'_' << v;
                        out << " )\n";

                    }
    // each two cases in a column should be different
    for(int c = 0; c<9; c++)
        for(int l1 = 0; l1<9; l1++)
            for(int l2 = l1+1; l2<9; l2++)
                for(int v = 0; v<9; v++)
                    {
                        out << "( ";
                        out << "V_" << l1 << '_'<< c <<'_' << v;
                        out << " & !";
                        out << "V_" << l2 << '_'<< c <<'_' << v;
                        out << " ) | ( !";
                        out << "V_" << l1 << '_'<< c <<'_' << v;
                        out << " & ";
                        out << "V_" << l2 << '_'<< c <<'_' << v;
                        out << " )\n";
                    }

    // each two cases in a square should be different
    for(int sq = 0; sq<9; sq++)
        for(int c1 = 0; c1<9; c1++)
            for(int c2 = c1+1; c2<9; c2++)
                for(int v = 0; v<9; v++)
                    {
                        int x1, x2, y1, y2;
                        convertSqr(sq, c1, x1, y1);
                        convertSqr(sq, c2, x2, y2);
                        out << "( ";
                        out << "V_" << x1 << '_'<< y1 <<'_' << v;
                        out << " & !";
                        out << "V_" << x2 << '_'<< y2 <<'_' << v;
                        out << " ) | ( !";
                        out << "V_" << x1 << '_'<< y2 <<'_' << v;
                        out << " & ";
                        out << "V_" << x2 << '_'<< y2  <<'_' << v;
                        out << " )\n";

                    }
}

// format the output
std::string format(int in, int sz)
{
    // result imposed ? set bracets
    bool br = in>=0;
    // else give the right value
    if(in<0)
        in = -in-2;
    // size of the entry
    int csz = 1;
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

void CSudoku::translateOut(std::istream& out, std::ostream& result)
{
    std::string parse;
    while(parse != "s")
        out >> parse;
    out >> parse;

    // check if the given square is solvable
    if(parse == "UNSATISFIABLE")
    {
        result << "Unsolvable sudoku\n";
        return;
    }

    // parse the result and store them into the table
    while(out.good())
    {
        while(parse != "v")
            out >> parse;
        out >> parse;
        // if we have a positive assignationint** CLatin::m_board = 0;

        if (parse[0] != '!')
        {
            int r, c, v;
            std::stringstream cnv;
            for(int cur = 2; cur < (int)parse.size(); cur++)
                if(parse[cur] == '_')
                    cnv << ' ';
                else
                    cnv << parse[cur];

            cnv >> r >> c >> v;
            if(m_board[r][c] == -1)
                m_board[r][c] = -v-2; // less than -2 to remeber it was not imposed
        }
    }

    // now we can draw the result
    int sz = 3; // size of each square
    for(int i = 0; i<9; i++)
    {
        for(int j = 0; j<9; j++)
        {
            result << format(m_board[i][j], sz);
            if(j == 2 || j == 5)
                result << '|';
        }
        if(i == 2 || i == 5)
            result << "\n---------+---------+---------";

        result << "\n";
    }
}

int CSudoku::m_board[9][9]/* = {0}*/;
