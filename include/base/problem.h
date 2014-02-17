#ifndef PROBLEM_BASE_H
#define PROBLEM_BASE_H

#include <cmath>
#include <vector>
#include <iostream>
#include <stack>

#ifdef _DEBUG
#include <fstream>
#endif

#include "base/container.h"
#include "base/literal.h"
#include "base/sat.h"

#include "translate.h"

// variables of problem
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CProblem_base_static_vars
{
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
public:
    virtual ~CProblem_base_static_vars(){}
protected:
    // static sat : the same for all classes.
    static CSAT<_ALG, _WAT, _CL> static_sat;
    static bool static_isSat;
    static bool static_found;

    // assign the litteral and makes the linked simplifications
    void simplify(Literal);

    // contains the modifications to the SAT problem
    std::list<typename VariablePtr<_ALG, _WAT, _CL>::type> m_assigned;
};

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CProblem_conflict : virtual public CProblem_base_static_vars<_ALG, _WAT, _CL>
{
public:
    virtual ~CProblem_conflict(){}
    // update the variable of the conflict
    void updateProblem(){}
    void restoreProblem(){}
    // say if we should continue to backtrack or if we must continue
    bool mustBacktrack(){return false;}
    // set a deduced literal
    void setDeducedLit(CLiteral<_ALG, _WAT, _CL>, typename ClausePtr<_ALG, _WAT, _CL>::type){}
    // set a bet literal
    void setBetLit(CLiteral<_ALG, _WAT, _CL>){}
    // handle the conflict
    void handleConflict(){}
};

// debug base
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CProblem_debug_base : virtual public CProblem_base_static_vars<_ALG, _WAT, _CL>
{
    using CProblem_base_static_vars<_ALG, _WAT, _CL>::static_sat;
public:
    virtual ~CProblem_debug_base(){}
protected:
    // debug print
    void debug();

    #ifdef _DEBUG
    static std::ofstream dbgFile;
    #endif
};

// base class that can be specialized in algotithms
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CProblem_base
{
public:
    virtual ~CProblem_base(){}
};

// class for a problem
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CProblem : virtual public CProblem_base<_ALG, _WAT, _CL>,
                 virtual public CProblem_debug_base<_ALG, _WAT, _CL>,
                 virtual public CProblem_base_static_vars<_ALG, _WAT, _CL>,
                 public CProblem_conflict<_ALG, _WAT, _CL>
{

    typedef CLiteral<_ALG, _WAT, _CL> Literal;

public:

    using CProblem_base_static_vars<_ALG, _WAT, _CL>::static_sat;
    using CProblem_base_static_vars<_ALG, _WAT, _CL>::static_isSat;
    using CProblem_base_static_vars<_ALG, _WAT, _CL>::static_found;
    using CProblem_base_static_vars<_ALG, _WAT, _CL>::m_assigned;

    CProblem() {}
    ~CProblem() {}

    // launcher of the recursive solve
    void launchRecSolve();

    // the problem class is mainly static, we can provide a method that does all the job for us;
    static void solve(std::istream& _in, std::ostream& _out)
    {
        CProblem prob;
        _in >> prob;
        prob.launchRecSolve();
        _out << prob;
    }

    // friend IO functions
    template<SATheuristic friend_ALG, LitChoice friend_WAT, ClauseChoice friend_CL>
    friend std::istream& operator >> (std::istream& , CProblem<friend_ALG, friend_WAT,  friend_CL>&);

    template<SATheuristic friend_ALG, LitChoice friend_WAT, ClauseChoice friend_CL>
    friend std::ostream& operator << (std::ostream& , CProblem<friend_ALG, friend_WAT, friend_CL>);


    // recursive solve
    // argument : parent problem and literal assigned
    void recSolve(Literal);

protected:
    // input errors
    std::string m_error;
    std::stack<Translate> m_translate;


};

// error strings
const std::string err_header = "En tete invalide";
const std::string err_h_too_short = "En tete incomplet";
const std::string err_too_short = "Fichier incomplet";


// ========================================================================================================
//                                    IMPLEMENTATION
// ========================================================================================================

// assign the litteral and makes the linked simplifications
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CProblem_base_static_vars<_ALG, _WAT, _CL>::simplify(CLiteral<_ALG, _WAT, _CL> l)
{
    // remember the var assigned and assign it
    m_assigned.push_back(l.variable());
    if(!static_sat.assignVar(l.variable(), l.logicValue()))
        static_found = true; // we found that the assignassion is not satisfiable
}

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CProblem<_ALG, _WAT, _CL>::launchRecSolve()
{
    // do nothing is there is an input error
    if (m_error != "")
        return;

    // simplification
    CProblem_debug_base<_ALG, _WAT, _CL>::debug();

    // pointer to the clause to simplify
    typename ClausePtr<_ALG, _WAT, _CL>::type pClause;

    // literal deducted
    CLiteral<_ALG, _WAT, _CL> changed;

    // boolean to continue while where are deductions
    bool isDeduct = true;

    while(isDeduct)
    {
        isDeduct = false;
        // get all singletons
        for (pClause = static_sat.clauses().begin(); !isDeduct && pClause!= static_sat.clauses().end();)
            if(pClause->deduction(changed))
            {
                // the clause is a singleton, simplify with it
                CProblem_base_static_vars<_ALG, _WAT, _CL>::simplify(changed);

                // set the leteral as a deduced one
                CProblem_conflict<_ALG, _WAT, _CL>::setDeducedLit(changed, pClause);

                // we found a conflict
                if(static_found)
                    CProblem_conflict<_ALG, _WAT, _CL>::handleConflict();

                // the itreators are no longer valid, restart the loop
                isDeduct = true;
            }
            else
                pClause++;
        // we may stop here if there is a conflict
        if(static_found)
            break;
    }

    CProblem_debug_base<_ALG, _WAT, _CL>::debug();

    if (static_found)
        return;

    // test if the problem is satisfiable
    static_isSat = static_sat.test();
    if(static_isSat) // is yes then we have nothing else to do
    {
        static_found = true;
        return;
    }


    // else propagate the recursive solution
    // next var for next problem
    CLiteral<_ALG, _WAT, _CL> l = static_sat.nextLit();
    if(l.variable() == static_sat.getVoidVar())
        return;

    // recursive solve with the two possible value of the variable
    CProblem<_ALG, _WAT, _CL> prob;
    prob.recSolve(l);
    if(!CProblem_conflict<_ALG, _WAT, _CL>::mustBacktrack())
        prob.recSolve(!l);
}


template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CProblem<_ALG, _WAT, _CL>::recSolve(CLiteral<_ALG, _WAT, _CL> l)
{
    // all the work is already done if isSat is true
    if (static_isSat)
        return;

    // clear modifications storage
    m_assigned.clear();

    // update the conflict handler
    CProblem_conflict<_ALG, _WAT, _CL>::updateProblem();

    // simplify according to the assigned litteral
    CProblem_base_static_vars<_ALG, _WAT, _CL>::simplify(l);

    // set the literal as a bet one
    CProblem_conflict<_ALG, _WAT, _CL>::setBetLit(l);

    // makes the rest of the simpl and recurse the solving
    if(!static_found)
        launchRecSolve();

    // restore SAT as before the solving it is not satisfiable
    if(!static_isSat)
    {
        // the next problem is maybe ok
        static_found = false;

        //var assigned
        for (typename std::list<typename VariablePtr<_ALG, _WAT, _CL>::type>::iterator it = m_assigned.begin(); it != m_assigned.end(); it++)
            static_sat.unassignVar(*it);

        // update the conflict handler
        CProblem_conflict<_ALG, _WAT, _CL>::restoreProblem();
    }
    return;
}

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CProblem_debug_base<_ALG, _WAT, _CL>::debug()
{
#ifdef _DEBUG
    static int step = 0;
    dbgFile << "Step : " << step++ << " ----------------------------------------------\n";
    dbgFile << "Sat :\n";
    dbgFile << static_sat;
    dbgFile << "Var : ";
    typename VarList<_ALG, _WAT, _CL>::type vars = static_sat.variables();
    for(typename VarList<_ALG, _WAT, _CL>::type::iterator it = vars.begin(); it != vars.end(); it++)
        if(it->isAssigned())
            dbgFile << "(" << it->name() << " = " << ((it->assignement())?"T":"F")<< ") ";
    dbgFile << std::endl;
#endif
}

namespace __CProblem_util__
{
    // convert string s into int i, return true if the conversion is vailid
    inline bool isInt(std::string s, int &i)
    {
        int r = 0;
        int d = 0;
        if (s[0] == '-')
            d = 1;
        for(int k = d; k<(int)s.size(); k++)
            if (s[k] < '0' || s[k] > '9')
                return false;
            else
                r = r*10+(s[k]-'0');
        i = r;
        if(d)
            i = -i;
        return true;
    }

    // convert int i to a string
    inline std::string itos(int i)
    {
        std::string ret;
        std::string sgn = (i<0)?"-":"";
        char tmp[2];
        tmp[1]=0;
        while(i)
        {
            tmp[0] = i%10+'0';
            ret = std::string(tmp) + ret;
            i /= 10;
        }
        return sgn+ret;
    }
}

// input function
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
std::istream& operator >> (std::istream& in, CProblem<_ALG, _WAT, _CL>& prob)
{
    // include previous utility def
    using namespace __CProblem_util__;

    // reinit sat
    prob.static_sat.init();
    prob.static_isSat = false;
    prob.static_found = false;

    // no error
    prob.m_error = "";
    // headers string
    std::string p, type;

    // parse all comments before the header
    p = "c";
    while (p == "c")
    {
        in >> p;

        if (p=="c")
            while(in.get() != '\n'); // until and of line
    }
    in >> type;

    if (p != "p")
    {

        prob.m_error = err_header;
        return in;
    }

    if(type == "latin")
    {
        std::stringstream sout;
        CLatin::translateIn(in, sout);
        prob.m_translate.push(latin);
        sout >> prob;

        return in;
    }
    else if(type == "magic")
    {
        std::stringstream sout;
        CMagic::translateIn(in, sout);
        prob.m_translate.push(magic);
        sout >> prob;

        return in;
    }
    else if(type == "sudoku")
    {
        std::stringstream sout;
        CSudoku::translateIn(in, sout);
        prob.m_translate.push(sudoku);
        sout >> prob;

        return in;
    }
    else if(type == "arith")
    {
        std::stringstream sout;
        CArith::translateIn(in, sout);
        prob.m_translate.push(arith);
        sout >> prob;

        return in;
    }
    else if(type == "logic")
    {
        std::stringstream sout;

        CTseitin::translateIn(in, sout);
        prob.m_translate.push(tseitin);
        //std::cerr << sout.str();
        sout >> prob;
        return in;

    }
    else if(type == "smt")
    {
        std::stringstream sout;
        SMT::translateIn(in, sout);
        prob.m_translate.push(smt);
        sout >> prob;
        return in;

    }
    else if (type == "edge"){
        std::stringstream sout;
        CKColor::translateIn(in, sout);
        prob.m_translate.push(kcolor);
        sout >> prob;
        return in;
    }
    else if(type != "cnf")
    {
        prob.m_error = err_header;
        return in;
    }

    prob.m_translate.push(cnf);

    // number of var - clauses
    int V, C;

    //current string
    std::string cur;

    // number of vars
    if(!(in >> cur))
    {
        prob.m_error = err_h_too_short;
        return in;
    }
    if(!isInt(cur, V))
    {
        prob.m_error = "Le 3eme mot de l'en-tete doit etre un entier represenatant le nombre de variables";
        return in;
    }
    if (V<=0)
    {
        prob.m_error = "Le nombre de variables doit être strictement postif";
        return in;
    }

    //  "void" var
    typename VariablePtr<_ALG, _WAT, _CL>::type voidVar = prob.static_sat.getVoidVar();
    // vector to convert the int to vars
    std::vector<typename VariablePtr<_ALG, _WAT, _CL>::type> variables(V+1, voidVar);

    // number of clauses
    if(!(in >> cur))
    {
        prob.m_error = err_h_too_short;
        return in;
    }
    if (!isInt(cur, C))
    {
        prob.m_error = "Le 4eme mot de l'en-tete doit etre un entier represenatant le nombre de clauses";
        return in;
    }
    if (C<=0)
    {
        prob.m_error = "Le nombre de clauses doit être strictement postif";
        return in;
    }

    // i = input int
    int i = 0;
    // parsing of clauses
    for(int k = 0; k<C; k++)
    {
        if (!(in >> cur)) // EOF
        {
            prob.m_error = err_too_short;
            std::cerr << k <<' ' << C << '\n';
            return in;
        }
        // comment
        if (cur == "c")
            while(in.get() != '\n');
        else
        {
            // creation of the clause
            CClause<_ALG, _WAT, _CL> tmpClause;

            //current litteral
            CLiteral<_ALG, _WAT, _CL> curLiteral(voidVar, true);

            if (!isInt(cur, i))
            {
                prob.m_error = "Une des variables n'est pas un entier";
                return in;
            }
            while(i)
            {
                if(i<-V || i>V)
                {
                    prob.m_error = "Une variable est hors limite";
                    return in;
                }
                if (variables[std::abs(i)] == voidVar)
                    variables[std::abs(i)] = prob.static_sat.newVar(itos(std::abs(i)));
                curLiteral.variable() = variables[std::abs(i)];
                curLiteral.logicValue() = (i>0);

                tmpClause.addLiteral(curLiteral);

                if(!(in>>cur))
                {
                    prob.m_error = err_too_short;
                    return in;
                }

                if (!isInt(cur, i))
                {
                    prob.m_error = "Une des variables n'est pas un entier";
                    return in;
                }
            }
            prob.static_sat.addClause(tmpClause);
        }
    }
    return in;
}

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
std::ostream& operator << (std::ostream& out, CProblem<_ALG, _WAT, _CL> prob)
{
    // error
    if (prob.m_error != "")
    {
        out << "c Error : " << prob.m_error << std::endl;
        return out;
    }

    std::stringstream tmp_out;
    //std::stringstream actualResult;
    while(!prob.m_translate.empty())
    {
        //actualResult.str("");
        Translate t = prob.m_translate.top();

        prob.m_translate.pop();
        std::stringstream cnv;
        switch(t)
        {
        case cnf:
            if (prob.static_isSat)
            {
                cnv << "s SATISFIABLE\n";
                // we can show the assignement
                typename VarList<_ALG, _WAT, _CL>::type vars = prob.static_sat.variables();
                for(typename VarList<_ALG, _WAT, _CL>::type::iterator it = vars.begin(); it != vars.end(); it++)
                    if(it->isAssigned())
                        cnv << "v " << ((it->assignement())?"":"-")<<it->name() << std::endl;
            }
            // unsatifiable
            else
                cnv << "s UNSATISFIABLE\n";
            break;

        case tseitin:
            CTseitin::translateOut(tmp_out, cnv);
            break;
        case latin:
            CLatin::translateOut(tmp_out, cnv);
            break;
        case magic:
            CMagic::translateOut(tmp_out, cnv);
            break;
        case sudoku:
            CSudoku::translateOut(tmp_out, cnv);
            break;
        case arith:
            CArith::translateOut(tmp_out, cnv);
            break;
        case smt:
            if(SMT::check(tmp_out, cnv))
            {
                CProblem<_ALG, _WAT, _CL> newProb;
                newProb.m_translate.push(smt);
                std::stringstream tmp(SMT::tseitinStream);
                newProb.solve(tmp, cnv);
            }
            break;
         case kcolor:
            CKColor::translateOut(tmp_out, cnv);
            break;
        }
        //actualResult << cnv.str();
        tmp_out.str("");
        tmp_out.clear();

        tmp_out << cnv.str();


    }

    //out << actualResult.str();
    out << tmp_out.str();

    return out;
}

// static implementations

// data of the SAT problem
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    CSAT<_ALG, _WAT, _CL> CProblem_base_static_vars<_ALG, _WAT, _CL>::static_sat =  CSAT<_ALG, _WAT, _CL>();

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    bool CProblem_base_static_vars<_ALG, _WAT, _CL>::static_isSat = false; // is the problem satifiable ?

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    bool CProblem_base_static_vars<_ALG, _WAT, _CL>::static_found = false; // do we have a solution

#ifdef _DEBUG
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    std::ofstream CProblem_debug_base<_ALG, _WAT, _CL>::dbgFile("debug.log");
#endif

#endif // PROBLEM_BASE_H
