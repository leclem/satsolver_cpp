#ifndef WATCHED_SAT_H
#define WATCHED_SAT_H

#include "base/sat.h"

#include "watched/clause.h"


template<SATheuristic _ALG, ClauseChoice _CL>
class CSAT_base<_ALG, WATCHED, _CL> : virtual public CSAT_base_vars<_ALG, WATCHED, _CL>
{
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_assignedClauses;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_unassignedClauses;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_vars;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_assignedVars;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_unassignedVars;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_emptyList;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_voidVar;
    using CSAT_base_vars<_ALG, WATCHED, _CL>::m_conflictClause;

    typedef CVariable<_ALG, WATCHED, _CL> Variable;
    typedef CLiteral<_ALG, WATCHED, _CL> Literal;
    typedef CClause<_ALG, WATCHED, _CL> Clause;
    typedef typename VariablePtr<_ALG, WATCHED, _CL>::type VariablePtr;
    typedef typename ClausePtr<_ALG, WATCHED, _CL>::type ClausePtr;

    typedef CClauseInfo<_ALG, WATCHED, _CL> ClauseInfo;
    typedef std::list<ClauseInfo> ClInfoList;
    typedef typename ClInfoList::iterator ClInfoListIter;

    typedef typename VarList<_ALG, WATCHED, _CL>::type VarLst;
    typedef typename SATContener<_ALG, WATCHED, _CL>::type SATCont;

public:
    // clauses operations
    ClausePtr addClause(Clause);

    // update the structures when we assign or unassign a variable
    bool assignVar(VariablePtr, bool);
    void unassignVar(VariablePtr);
};

/// =================================== IMPLEMENTAION ===================================
/**
//First passage
void isVariableWithOnlyOneAssignement(){

    //We search if the variable only have one polarity
    for (typename VarList<_ALG, _WAT, _CL>::type::iterator it = variables().begin(); it != variables().end(); it++){
        VarPtr currentVar = (*it);
        bool litteralsTrueIsEmpty = currentVar->getClauses(true).empty();
        bool litteralsFalseIsEmpty = currentVar->getClauses(false).empty();
        if (litteralsTrueIsEmpty && !litteralsFalseIsEmpty){
            currentVar->assigne(false);
        }
        else if (!litteralsTrueIsEmpty && litteralsFalseIsEmpty){
            currentVar->assigne(true);
        }
    }

}
*/


//add a clause
template<SATheuristic _ALG, ClauseChoice _CL>
typename ClausePtr<_ALG, WATCHED, _CL>::type CSAT_base<_ALG, WATCHED, _CL>::addClause(Clause _clause)
{
    // if the clause is trivial, we do not add it
    if(_clause.isTrivial())
        return m_assignedClauses.end();

    ClausePtr p_c;

    // add to clause list
    m_unassignedClauses.push_front(_clause);
    p_c = m_unassignedClauses.begin();

    // init the watches
    p_c->initWatch();

    // rectify clause list if the clause was assigned
    if(p_c->isVerified())
        m_assignedClauses.splice(m_assignedClauses.begin(), m_unassignedClauses, p_c);

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
bool CSAT_base<_ALG, WATCHED, _CL>::assignVar(VariablePtr _p, bool a)
{
    bool ret = true;

    //update clauses with good assignations
    for(ClInfoListIter info_it = _p->getClauses(a).begin();  info_it != _p->getClauses(a).end(); info_it++ )
    {
        ClausePtr p_c = info_it->clause;

        // if it is already sat, we do nothing
        if(p_c->isVerified())
            continue;

        // else we just give the litteral to watch
        p_c->setWatch(info_it->pos);

        //and we put it in the assigned list
        m_assignedClauses.splice(m_assignedClauses.begin(), m_unassignedClauses, p_c);
    }

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

    //update the informations for the non matching watched literals
    for(ClInfoListIter info_it = _p->getClauses(!a).begin();  info_it != _p->getClauses(!a).end(); info_it++ )
    {
        ClausePtr p_c = info_it->clause;

        // we just want the watched literals
        if(!p_c->watch(info_it->pos))
            continue;

        // if the clause is already satisfiable, there is nothing to do
        if(p_c->isVerified())
            continue;

        // update the clauses informations because literals are not all in the correct part of the clause
        p_c->update();

        // then watch the next literals if it is possible, else say that there is a conflic
        if(!p_c->watchNext(info_it->pos))
        {
            m_conflictClause = p_c;
            ret = false;
        }
    }
    return ret;
}

// unassign an assigned var
template<SATheuristic _ALG, ClauseChoice _CL>
void CSAT_base<_ALG, WATCHED, _CL>::unassignVar(VariablePtr _p)
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

    // update the clauses with the good assignment
    for(ClInfoListIter info_it = _p->getClauses(a).begin();  info_it != _p->getClauses(a).end(); info_it++ )
        if(!info_it->clause->isVerified())
            m_unassignedClauses.splice(m_unassignedClauses.begin(), m_assignedClauses, info_it->clause);
}


#endif // WATCHED_SAT_H
