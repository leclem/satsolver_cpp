#ifndef WATCHED_CLAUSE_H
#define WATCHED_CLAUSE_H

#include "base/variable.h"
#include "base/literal.h"
#include "base/clause.h"

// class that can be inherited by clauses to specialize
template<SATheuristic _ALG, ClauseChoice _CL>
class CClause_base<_ALG, WATCHED, _CL>: virtual public CClause_vars<_ALG, WATCHED, _CL>
{
    typedef CLiteral<_ALG, WATCHED, _CL> Literal;
    typedef typename std::list<Literal>::iterator LitIter;
protected:
    // Literal type
    using CClause_vars<_ALG, WATCHED, _CL>::m_unassignedLit;
    using CClause_vars<_ALG, WATCHED, _CL>::m_goodAssignedLit;
    using CClause_vars<_ALG, WATCHED, _CL>::m_wrongAssignedLit;
public:

    // return true if the given variable is watched
    bool watch(LitIter it)
        { return w0 == it || w1 == it; }

    // give the first watched litterals
    void initWatch();

    //update the variables in the clause
    void update();

    // is the clause satisfied according to the watched literals
    bool isVerified();

    // set the next litteral to watch after the assignation of arg0
    // return true if a new litteral was found
    bool watchNext(LitIter it);

    // set litteral as watched ones, if possible one that is not good
    void setWatch(LitIter it)
    {
        if(w0->variable()->isAssigned())
        {
            if(w0->variable()->assignement() == w0->logicValue())
                w1 = it;
        }
        else
            w0 = it;
    }

    bool deduction(Literal&);

    LitIter watch0() {return w0;}
    LitIter watch1() {return w1;}

private:
    LitIter w0, w1;
};

/// ================================= IMPLEMENTATION  ========================================

// set the first watched litterals
template<SATheuristic _ALG, ClauseChoice _CL>
void CClause_base<_ALG, WATCHED, _CL>::initWatch()
{
    // set in priority a good assigned literal, then an unassigned, and at last a wrongly assigned one
    if(!m_goodAssignedLit.empty())
    {
        w0 = m_goodAssignedLit.begin();
        if(m_goodAssignedLit.size()>1)
        {
            w1 = w0;
            w1++;
        }
        else if(!m_unassignedLit.empty())
            w1 = m_unassignedLit.begin();
        else if(!m_wrongAssignedLit.empty())
            w1 = m_wrongAssignedLit.begin();
        else
            w1 = w0;
    }
    else if(!m_unassignedLit.empty())
    {
        w0 = m_unassignedLit.begin();
        if(m_unassignedLit.size()>1)
        {
            w1 = w0;
            w1++;
        }
        else if(!m_wrongAssignedLit.empty())
            w1 = m_wrongAssignedLit.begin();
        else
            w1 = w0;
    }
    else if(!m_wrongAssignedLit.empty())
    {
        w0 = m_wrongAssignedLit.begin();
        if(m_wrongAssignedLit.size()>1)
        {
            w1 = w0;
            w1++;
        }
        else
            w1 = w0;
    }
}

//update the variables in the clause
template<SATheuristic _ALG, ClauseChoice _CL>
void CClause_base<_ALG, WATCHED, _CL>::update()
{
    std::list<Literal> tmpUnassigned, tmpGood, tmpWrong;

    for(LitIter it = m_unassignedLit.begin(); it != m_unassignedLit.end();)
    {
        LitIter tmp = it;
        it++;
        if(tmp->variable()->isAssigned())
        {
            if(tmp->variable()->assignement() == tmp->logicValue())
                tmpGood.splice(tmpGood.begin(), m_unassignedLit, tmp);
            else
                tmpWrong.splice(tmpWrong.begin(), m_unassignedLit, tmp);
        }
    }
    for(LitIter it = m_wrongAssignedLit.begin(); it != m_wrongAssignedLit.end();)
    {
        LitIter tmp = it;
        it++;
        if(!tmp->variable()->isAssigned())
            tmpUnassigned.splice(tmpUnassigned.begin(), m_wrongAssignedLit, tmp);
        else if(tmp->variable()->assignement() == tmp->logicValue())
            tmpGood.splice(tmpGood.begin(), m_wrongAssignedLit, tmp);
    }
    for(LitIter it = m_goodAssignedLit.begin(); it != m_goodAssignedLit.end();)
    {
        LitIter tmp = it;
        it++;
        if(!tmp->variable()->isAssigned())
            tmpUnassigned.splice(tmpUnassigned.begin(), m_goodAssignedLit, tmp);
        else if(tmp->variable()->assignement() != tmp->logicValue())
            tmpGood.splice(tmpGood.begin(), m_goodAssignedLit, tmp);
    }

    m_unassignedLit.splice(m_unassignedLit.begin(), tmpUnassigned);
    m_goodAssignedLit.splice(m_goodAssignedLit.begin(), tmpGood);
    m_wrongAssignedLit.splice(m_wrongAssignedLit.begin(), tmpWrong);
}

// is the clause satisfied according to the watched literals
template<SATheuristic _ALG, ClauseChoice _CL>
bool CClause_base<_ALG, WATCHED, _CL>::isVerified()
{
    if(w0->variable()->isAssigned())
        if(w0->variable()->assignement() == w0->logicValue())
            return true;
    if(w1->variable()->isAssigned())
        if(w1->variable()->assignement() == w1->logicValue())
            return true;
    return false;
}

template<SATheuristic _ALG, ClauseChoice _CL>
bool CClause_base<_ALG, WATCHED, _CL>::watchNext(LitIter it)
{
    LitIter*   changed = (it==w0)? &w0:&w1;
    LitIter* unchanged = (it==w0)? &w1:&w0;

    if(!m_goodAssignedLit.empty() && *unchanged != m_goodAssignedLit.begin())
        *changed = m_goodAssignedLit.begin();
    else if(m_goodAssignedLit.size()>1)
        *changed = ++m_goodAssignedLit.begin();
    else if(!m_unassignedLit.empty() && *unchanged != m_unassignedLit.begin())
        *changed = m_unassignedLit.begin();
    else if(m_unassignedLit.size()>1)
        *changed = ++m_unassignedLit.begin();
     // only uncorrect left, conflict if the unchanged is already wrongly assigned
    else if (!(*unchanged)->variable()->isAssigned())
        return true;
    else
        return (*unchanged)->variable()->assignement() == (*unchanged)->logicValue();
    return true;
}

template<SATheuristic _ALG, ClauseChoice _CL>
bool CClause_base<_ALG, WATCHED, _CL>::deduction(Literal& deducted)
{
    // no deduction if the clause is verified
    if(isVerified())
        return false;
    // no deduction if there is still two unassigned vars
    if(!w0->variable()->isAssigned() && !w1->variable()->isAssigned())
        return false;
    //else, conflict
    if(w0->variable()->isAssigned())
    {
        if(w0->variable()->assignement() != w0->logicValue())
        {
            deducted = *w1;
            return true;
        }
    }
    if(w1->variable()->isAssigned())
    {
        if(w1->variable()->assignement() != w1->logicValue())
        {
            deducted = *w0;
            return true;
        }
    }
    return false;
}

template<SATheuristic _ALG, ClauseChoice _CL>
    std::ostream& operator << (std::ostream& out , CClause<_ALG, WATCHED, _CL>& c)
{
    if (c.unassigneds().empty() && c.assigneds(true).empty() && c.assigneds(false).empty())
    {
        out << "[ empty clause ]";
        return out;
    }

    out << "[ size = " << c.assigneds(true).size() + c.assigneds(false).size() + c.unassigneds().size();
    out << "; w0 = ";
    if(!c.watch0()->variable()->isAssigned())
        out << "free";
    else if(c.watch0()->variable()->assignement() == c.watch0()->logicValue())
        out << "sat";
    else
        out << "unsat";
    out << "( " << *c.watch0() << " )";

    if(c.watch0() != c.watch1())
    {
        out << "; w1 = ";
        if(!c.watch1()->variable()->isAssigned())
            out << "free";
        else if(c.watch1()->variable()->assignement() == c.watch1()->logicValue())
            out << "sat";
        else
            out << "unsat";
        out << "( " << *c.watch1() << " )";
    }
    out << " ]";
    return out;
}


#endif // WATCHED_CLAUSE_H
