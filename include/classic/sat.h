#ifndef CLASSIC_SAT_H
#define CLASSIC_SAT_H

#include "base/sat.h"

template<SATheuristic _ALG, ClauseChoice _CL>
class CSAT_base<_ALG, CLASSIC, _CL> : virtual public CSAT_base_vars<_ALG, CLASSIC, _CL>
{
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_assignedClauses;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_unassignedClauses;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_vars;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_assignedVars;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_unassignedVars;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_emptyList;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_voidVar;
    using CSAT_base_vars<_ALG, CLASSIC, _CL>::m_conflictClause;

    typedef CVariable<_ALG, CLASSIC, _CL> Variable;
    typedef CLiteral<_ALG, CLASSIC, _CL> Literal;
    typedef CClause<_ALG, CLASSIC, _CL> Clause;
    typedef typename VariablePtr<_ALG, CLASSIC, _CL>::type VariablePtr;
    typedef typename ClausePtr<_ALG, CLASSIC, _CL>::type ClausePtr;

    typedef CClauseInfo<_ALG, CLASSIC, _CL> ClauseInfo;
    typedef std::list<ClauseInfo> ClInfoList;
    typedef typename ClInfoList::iterator ClInfoListIter;

    typedef typename VarList<_ALG, CLASSIC, _CL>::type VarLst;
    typedef typename SATContener<_ALG, CLASSIC, _CL>::type SATCont;

public:
    // clauses operations
    ClausePtr addClause(Clause);

    // update the structures when we assign or unassign a variable
    bool assignVar(VariablePtr, bool);
    void unassignVar(VariablePtr);
};

/// =================================== IMPLEMENTAION ===================================

//add a clause
template<SATheuristic _ALG, ClauseChoice _CL>
typename ClausePtr<_ALG, CLASSIC, _CL>::type CSAT_base<_ALG, CLASSIC, _CL>::addClause(Clause _clause)
{
    // if the clause is trivial, we do not add it
    if(_clause.isTrivial())
        return m_assignedClauses.end();

    ClausePtr p_c;

    // add to clause list
    if(_clause.isVerified())
    {
        m_assignedClauses.push_front(_clause);
        p_c = m_assignedClauses.begin();
    }
    else
    {
        m_unassignedClauses.push_front(_clause);
        p_c = m_unassignedClauses.begin();
    }

    // update all contained vars
    for(typename std::list<Literal>::iterator lit_it = p_c->assigneds(true).begin(); lit_it != p_c->assigneds(true).end(); lit_it++)
        lit_it->variable()->addClauseInfo(ClauseInfo(p_c, lit_it), lit_it->logicValue());
    for(typename std::list<Literal>::iterator lit_it = p_c->assigneds(false).begin(); lit_it != p_c->assigneds(false).end(); lit_it++)
        lit_it->variable()->addClauseInfo(ClauseInfo(p_c, lit_it), lit_it->logicValue());
    for(typename std::list<Literal>::iterator lit_it = p_c->unassigneds().begin(); lit_it != p_c->unassigneds().end(); lit_it++)
        lit_it->variable()->addClauseInfo(ClauseInfo(p_c, lit_it), lit_it->logicValue());

    return p_c;
}

// assign a var, assuming it is not already assigned
// return true if there is no conflict at this time
template<SATheuristic _ALG, ClauseChoice _CL>
bool CSAT_base<_ALG, CLASSIC, _CL>::assignVar(VariablePtr _p, bool a)
{
    bool ret = true;
    // change the variables informations
    _p->assigne(a);
    if(m_unassignedVars.size()>1)
    {
        m_unassignedVars[_p->assignmentPos()] = m_unassignedVars[m_unassignedVars.size()-1];
        m_unassignedVars[_p->assignmentPos()]->assignmentPos() = _p->assignmentPos();
        m_unassignedVars.resize(m_unassignedVars.size()-1);
    }
    m_assignedVars.push_back(_p);
    _p->assignmentPos() = m_assignedVars.size()-1;

    // update the clauses containing a matching literal
    for(ClInfoListIter info_it = _p->getClauses(a).begin();  info_it != _p->getClauses(a).end(); info_it++)
    {
        ClausePtr p_c = info_it->clause;
        p_c->assigneds(true).splice(p_c->assigneds(true).begin(), p_c->unassigneds(), info_it->pos);

        // if the clause was not already verified, we put it in the assigned box
        if(p_c->assigneds(true).size()==1)
        {
            m_assignedClauses.splice(m_assignedClauses.begin(), m_unassignedClauses, p_c);

            //and we tell the clause to update some info
            p_c->setAssigned();
        }
    }

    // update the clauses containing a non matching literal
    for(ClInfoListIter info_it = _p->getClauses(!a).begin();  info_it != _p->getClauses(!a).end(); info_it++)
    {
        ClausePtr p_c = info_it->clause;
        p_c->assigneds(false).splice(p_c->assigneds(false).begin(), p_c->unassigneds(), info_it->pos);
        //check for conflict
        if(p_c->conflict())
        {
            m_conflictClause = p_c;
            ret = false;
        }
    }
    return ret;
}

// unassign an assigned var
template<SATheuristic _ALG, ClauseChoice _CL>
void CSAT_base<_ALG, CLASSIC, _CL>::unassignVar(VariablePtr _p)
{
    //remember the assignment
    bool a = _p->assignement();

    // change the variables informations
    _p->unassign();
    if(m_assignedVars.size()>1)
    {
        m_assignedVars[_p->assignmentPos()] = m_assignedVars[m_assignedVars.size()-1];
        m_assignedVars[_p->assignmentPos()]->assignmentPos() = _p->assignmentPos();
        m_assignedVars.resize(m_assignedVars.size()-1);
    }
    m_unassignedVars.push_back(_p);
    _p->assignmentPos() = m_unassignedVars.size()-1;

    // update the clauses containing a matching literal
    for(ClInfoListIter info_it = _p->getClauses(a).begin();  info_it != _p->getClauses(a).end(); info_it++ )
    {
        ClausePtr p_c = info_it->clause;
        p_c->unassigneds().splice(p_c->unassigneds().begin(), p_c->assigneds(true), info_it->pos);
        // if the clause is no more verified
        if(!p_c->isVerified())
        {
            m_unassignedClauses.splice(m_unassignedClauses.begin(), m_assignedClauses, p_c);

            //and we tell the clause to update some info
            p_c->setUnassigned();
        }
    }

    // update the clauses containing a non matching literal
    for(ClInfoListIter info_it = _p->getClauses(!a).begin();  info_it != _p->getClauses(!a).end(); info_it++ )
        info_it->clause->unassigneds().splice(info_it->clause->unassigneds().begin(), info_it->clause->assigneds(false), info_it->pos);

}
#endif // CLASSIC_SAT_H
