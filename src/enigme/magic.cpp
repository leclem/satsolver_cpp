#include <sstream>
#include <list>
#include <vector>

#include "enigme/magic.h"

// format:
// N M (square size; number of imposed squares)
// then M lines:
// x y v (avec x, y, v de 0 à N-1) : square[x][y] = v.
// ...

static int log2(int i)
{
    int r = 0;
    while(i)
    {
        i/=2;
        r++;
    }
    return r;
}

static std::string mkVar(std::string v, int i, int j, int k)
{
    std::stringstream out;
    out << v << '_' << i << '_' <<j <<'_';
    if(k>=0)
        out << k;
    std::string s;
    out >> s;
    return s;
}

static void mkSum(std::ostream& out, std::list<std::string>& lst, int mx_sz)
{
    static int var_c = 0;
    while(lst.size() > 1)
    {
        std::stringstream varName;
        if(lst.size() == 2) // the last created variable is the sum
            varName << "S";
        else // make a temporary var for the sum
            varName << "T" << var_c;
        std::stringstream ret;
        ret << "R" << var_c;

        // get the two vars to add
        std::string v0 = lst.front(); lst.pop_front();
        std::string v1 = lst.front(); lst.pop_front();

        // k = 0
        // carry
        out << ret.str() << "0 & " << v0 << "0 & " << v1 <<"0\n";
        // sum
        out << varName.str() << "0 & ("<< v0 << "0 xor " << v1 <<"0)\n";
        for(int k = 1; k<mx_sz; k++)
        {
            // carry
            out << ret.str() << k << " & ((" << v0 << k << " & " << v1 << k << ") | (" << v0 << k << " & " << ret.str() << k <<") | (";
            out << ret.str() << k << " & " << v1 << k << "))\n";
            // sum
            out << varName.str() << k << " & (" << v0 << k << " xor " << v1 << k << " xor " << ret.str() << k << ")\n";
        }

        // push back the new var to make a tree
        lst.push_back(varName.str());

        // ask for a new varName
        var_c++;
    }
}
void CMagic::translateIn(std::istream& in, std::ostream& out)
{
    int N, M;
    in >> N >> M;
    m_n = N;

    // size of the greatest possible number found (sum of n² in a row)
    int sz_max = log2(m_n*m_n*m_n);

    m_board = new int* [N];
    for(int i = 0; i<N; i++)
        m_board[i] = new int [N];

    for(int i = 0; i< N; i++)
        for(int j = 0; j<N; j++)
            m_board[i][j] = 0;

    // out header
    out << "p logic ";

    // variables : V_i_j_k -> bit k of square i j
    for(int i = 0; i<M; i++)
    {
        int a, b, v;
        in >> a  >> b >> v;
        m_board[a][b] = -v-1;

        // we impose the values
        std::vector<bool> bvec;
        while(v)
        {
            bvec.push_back(v%2);
            v /= 2;
        }
        // all greatest bits are 0
        for(int i = bvec.size( ); i<sz_max; i++)
            out << mkVar("!V", a, b, i) << std::endl;


        for(int i = 0; i<(int)bvec.size(); i++)
        {
            if(!bvec[bvec.size()-1-i])
                out << "!";
            out << mkVar("V", a, b, i) << std::endl;
        }
    }
    // each var in the square should be different
    for(int i = 0; i<N; i++)
        for(int j = 0; j<N; j++)
            for(int is = i; is<N; is++)
                for(int js = j+1; js<N; js++)
                {
                    for(int k = 0; k<sz_max; k++)
                    {
                        out << "(" << mkVar("V", i, j, k) << " & ";
                        out << "!" << mkVar("V", is, js, k) << ") | ";
                        out << "(!" << mkVar("V", i, j, k) << " & ";
                        out << mkVar("V", is, js, k) << ")";
                    }
                    out << '\n';
                }
    // sum of each line, column, and diagonal should be the same

    std::list<std::string> var_sum;

    for(int i=0; i<N; i++)
    {
        var_sum.clear();
        for(int j = 0; j<N; j++)
            var_sum.push_back(mkVar("V", i, j, -1));
        mkSum(out, var_sum, sz_max);
    }

    for(int i=0; i<N; i++)
    {
        var_sum.clear();
        for(int j = 0; j<N; j++)
            var_sum.push_back(mkVar("V", j, i, -1));
        mkSum(out, var_sum, sz_max);
    }

    var_sum.clear();
    for(int i=0; i<N; i++)
        var_sum.push_back(mkVar("V", i, i, -1));
    mkSum(out, var_sum, sz_max);

    var_sum.clear();
    for(int i=0; i<N; i++)
        var_sum.push_back(mkVar("V", i, N-i-1, -1));
    mkSum(out, var_sum, sz_max);
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

void CMagic::deleteSqr()
{
    for(int i = 0; i<m_n; i++)
        delete [] m_board[i];
    delete [] m_board;
}

// format the output
static std::string format(int in, int sz)
{
    // result imposed ? set bracets
    bool br = in<0;
    // give the right valuet
    if(br)
        in = -in-1;
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

void CMagic::translateOut(std::istream& out, std::ostream& result)
{
    std::cerr<<"ok\n";
    std::string parse;
    while(parse != "s")
        out >> parse;
    out >> parse;

    // check if the given square is solvable
    if(parse == "UNSATISFIABLE")
    {
        result << "Unsolvable magic square\n";
        return;
    }

    // parse the result and store them into the table
    while(out.good())
    {
        while(parse != "v")
            out >> parse;
        out >> parse;
        // if we have a positive assignation
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
            if(m_board[r][c] >= 0)
                m_board[r][c] += (2<<v);
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

int CMagic::m_n = 0;

int** CMagic::m_board = 0;
