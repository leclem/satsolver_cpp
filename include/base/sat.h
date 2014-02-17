#ifndef SAT_BASE_H
#define SAT_BASE_H

#include <iostream>
#include <vector>
#include "base/container.h"
#include "base/variable.h"

// class containing CSAT vars
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_vars
{
public:
    virtual ~CSAT_base_vars(){}

    typename SATContener<_ALG, _WAT, _CL>::type m_assignedClauses, m_unassignedClauses; // clauses

    typename VarList<_ALG, _WAT, _CL>::type m_vars;        // vars
    std::vector<typename VariablePtr<_ALG, _WAT, _CL>::type> m_assignedVars, m_unassignedVars;

    typename VariablePtr<_ALG, _WAT, _CL>::type m_voidVar; // "void" var
    typename VarList<_ALG, _WAT, _CL>::type m_emptyList;   // empty list for the void var

    typename ClausePtr<_ALG, _WAT, _CL>::type m_conflictClause;
};

// class that can be inherited by CSAT
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CSAT_base
{
    typedef CClause<_ALG, _WAT, _CL> Clause;
    typedef typename VariablePtr<_ALG, _WAT, _CL>::type VariablePtr;

public:
    // clauses operations
    typename ClausePtr<_ALG, _WAT, _CL>::type addClause(Clause){MUST_SPECIALIZE;}

    // update the structures when we assign or unassign a variable
    bool assignVar(VariablePtr, bool){MUST_SPECIALIZE;return true;}
    void unassignVar(VariablePtr){MUST_SPECIALIZE;}
};


// class to change nextVar behaviour
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_nextVar : virtual public CSAT_base_vars<_ALG, _WAT, _CL>
{
public:
    // next unassigned var
    CLiteral<_ALG, _WAT, _CL> nextLit()
        {MUST_SPECIALIZE; return CLiteral<_ALG, _WAT, _CL>();}
};

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CSAT : virtual public CSAT_base<_ALG, _WAT, _CL>, virtual public CSAT_base_vars<_ALG, _WAT, _CL>, public CSAT_base_nextVar<_ALG, _WAT, _CL>
{
public:

    using CSAT_base_vars<_ALG, _WAT, _CL>::m_assignedClauses;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_unassignedClauses;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_vars;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_assignedVars;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_unassignedVars;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_emptyList;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_voidVar;
    using CSAT_base_vars<_ALG, _WAT, _CL>::m_conflictClause;


    typedef CVariable<_ALG, _WAT, _CL> Variable;
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
    typedef CClause<_ALG, _WAT, _CL> Clause;
    typedef typename VariablePtr<_ALG, _WAT, _CL>::type VariablePtr;
    typedef typename ClausePtr<_ALG, _WAT, _CL>::type ClausePtr;

    typedef CClauseInfo<_ALG, _WAT, _CL> ClauseInfo;
    typedef std::list<ClauseInfo> ClInfoList;
    typedef typename ClInfoList::iterator ClInfoListIter;

    typedef typename VarList<_ALG, _WAT, _CL>::type VarLst;
    typedef typename SATContener<_ALG, _WAT, _CL>::type SATCont;

    CSAT() : CSAT_base<_ALG, _WAT, _CL>()
    {
        m_emptyList.push_back(Variable());
        m_voidVar = m_emptyList.end();
    }
    ~CSAT() {}

    // vars op

    // rembember a new var and return the corresponding ptr

    VariablePtr createVar(Variable v)
    {
        m_vars.push_front(v);
        VariablePtr p_v = m_vars.begin();
        if(p_v->isAssigned())
        {
            m_assignedVars.push_back(p_v);
            p_v->assignmentPos() = m_assignedVars.size()-1;
        }
        else
        {
            m_unassignedVars.push_back(p_v);
            p_v->assignmentPos() = m_unassignedVars.size()-1;
        }

        return p_v;
    }
    VariablePtr newVar()
        { return createVar(Variable()); }
    VariablePtr newVar(bool _b)
        { return createVar(Variable(_b)); }
    VariablePtr newVar(std::string _s)
        { return createVar(Variable(_s)); }
    VariablePtr newVar(std::string _s, bool _b)
        { return createVar(Variable(_s, _b)); }

    // give a "void", unused var
    VariablePtr getVoidVar()
        { return m_voidVar; }

    // return the list of vars
    VarLst& variables()
        { return m_vars;}

    std::vector<VariablePtr>& unassignedVars(){
        return m_unassignedVars;
    }

    // return the list of clauses
    SATCont& clauses()
        { return m_unassignedClauses; }

    // test if the problem is solved
    bool test()
        { return m_unassignedClauses.empty(); }

    void firstPass();

    void init();
    ClausePtr& getConflictClause() {return m_conflictClause;}
};

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
void CSAT<_ALG, _WAT, _CL>::init()
{
    m_assignedClauses.clear();
    m_unassignedClauses.clear();
    m_vars.clear();
    m_assignedVars.clear();
    m_unassignedVars.clear();
}


template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
std::ostream& operator << (std::ostream& out , CSAT<_ALG, _WAT, _CL>& s)
{
    for (typename SATContener<_ALG, _WAT, _CL>::type::iterator it = s.clauses().begin(); it != s.clauses().end(); it++)
        out << *it << std::endl;
    return out;
}


#endif // SAT_BASE_H
