#ifndef CLAUSE_BASE_H
#define CLAUSE_BASE_H

#include "base/container.h"
#include "base/literal.h"

// class for clauses vars
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CClause_vars
{
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
public:
    virtual ~CClause_vars(){}
protected:
    std::list<Literal> m_unassignedLit;
    std::list<Literal> m_goodAssignedLit;
    std::list<Literal> m_wrongAssignedLit;
};

// class that can be inherited by clauses to specialize
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CClause_base : virtual public CClause_vars<_ALG, _WAT, _CL>
{
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
protected:
    using CClause_vars<_ALG, _WAT, _CL>::m_unassignedLit;
    using CClause_vars<_ALG, _WAT, _CL>::m_goodAssignedLit;
    using CClause_vars<_ALG, _WAT, _CL>::m_wrongAssignedLit;
public:
    // return true if we can deduce something from a clause, and give the deduced literal
    bool deduction(Literal& lit)
    {
        // if the clause if verified, nothing to do.
        if(!m_goodAssignedLit.empty())
            return false;
        // else if the clause is singleton
        if(CClause_vars<_ALG, _WAT, _CL>::m_unassignedLit.size() == 1)
        {
            lit = CClause_vars<_ALG, _WAT, _CL>::m_unassignedLit.front();
            return true;
        }
        return false;
    }
    bool isVerified()
        { return !m_goodAssignedLit.empty();}
    void update(){}
};

// class for all clauses operations
template <SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CClause : public CClause_base<_ALG, _WAT, _CL>
{
    // Literal type
    typedef CLiteral<_ALG, _WAT, _CL> Literal;
    typedef typename std::list<Literal>::iterator LitIter;

protected:
    using CClause_vars<_ALG, _WAT, _CL>::m_unassignedLit;
    using CClause_vars<_ALG, _WAT, _CL>::m_goodAssignedLit;
    using CClause_vars<_ALG, _WAT, _CL>::m_wrongAssignedLit;

public:

    bool isTrivial()
    {
        // copy
        std::list<Literal> lit_list0 = m_unassignedLit;
        std::list<Literal> lit_list1 = m_goodAssignedLit;
        std::list<Literal> lit_list2 = m_wrongAssignedLit;
        lit_list0.splice(lit_list0.begin(), lit_list1);
        lit_list0.splice(lit_list0.begin(), lit_list1);
        lit_list0.sort();

        typename std::list<Literal>::iterator it1 = lit_list0.begin();
        typename std::list<Literal>::iterator it0 = it1++;

        for(;it1 != lit_list0.end();)
        {
            if(it0->variable() == it1->variable())
                return true;
            it0++;
            it1++;
        }
        return false;
    }

    std::list<Literal>& assigneds(bool good)
    {
        if(good)
            return m_goodAssignedLit;
        else
            return m_wrongAssignedLit;
    }
    std::list<Literal>& unassigneds()
        { return m_unassignedLit;}
    bool conflict()
        { return m_unassignedLit.empty() && m_goodAssignedLit.empty(); }

    bool contains(Literal _l)
    {
        typename std::list<Literal>::iterator it;
        for(it = m_unassignedLit.begin(); it!= m_unassignedLit.end(); it++)
            if (*it == _l)
                return true;
        for(it = m_goodAssignedLit.begin(); it!= m_goodAssignedLit.end(); it++)
            if (*it == _l)
                return true;
        for(it = m_wrongAssignedLit.begin(); it!= m_wrongAssignedLit.end(); it++)
            if (*it == _l)
                return true;
        return false;
    }

    void addLiteral(Literal _l)
    {
        if(contains(_l))
            return;
        if(!_l.variable()->isAssigned())
            m_unassignedLit.push_front(_l);
        else
        {
            if(_l.logicValue() == _l.variable()->assignement())
                m_goodAssignedLit.push_front(_l);
            else
                m_wrongAssignedLit.push_front(_l);
        }
    }

    void setAssigned()
    {
        for(LitIter it = m_unassignedLit.begin(); it != m_unassignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())--;
        for(LitIter it = m_goodAssignedLit.begin(); it != m_goodAssignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())--;
        for(LitIter it = m_wrongAssignedLit.begin(); it != m_wrongAssignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())--;
    }

    void setUnassigned()
    {
        for(LitIter it = m_unassignedLit.begin(); it != m_unassignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())++;
        for(LitIter it = m_goodAssignedLit.begin(); it != m_goodAssignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())++;
        for(LitIter it = m_wrongAssignedLit.begin(); it != m_wrongAssignedLit.end(); it++)
            it->variable()->numUnassignedClauses(it->logicValue())++;
    }
};

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    std::ostream& operator << (std::ostream& out , CClause<_ALG, _WAT, _CL>& c)
{
    if (c.unassigneds().empty() && c.assigneds(true).empty() && c.assigneds(false).empty())
    {
        out << "[ empty clause ]";
        return out;
    }

    out << "[ ";

    if (!c.assigneds(true).empty())
    {
        out << "sat(";
        typename std::list<CLiteral<_ALG, _WAT, _CL> >::iterator it = c.assigneds(true).begin();
        out << " " << *(it++) << " ";
        for (; it != c.assigneds(true).end(); it++)
            out << "| " << *it << " ";
        out << ") ";
    }

    if (!c.assigneds(false).empty())
    {
        out << "unsat(";
        typename std::list<CLiteral<_ALG, _WAT, _CL> >::iterator it = c.assigneds(false).begin();
        out << " " << *(it++) << " ";
        for (; it != c.assigneds(false).end(); it++)
            out << "| " << *it << " ";
        out << ") ";
    }

    if (!c.unassigneds().empty())
    {
        out << "free(";
        typename std::list<CLiteral<_ALG, _WAT, _CL> >::iterator it = c.unassigneds().begin();
        out << " " << *(it++) << " ";
        for (; it != c.unassigneds().end(); it++)
            out << "| " << *it << " ";
        out << ") ";
    }
    out << "]";
    return out;
}

#endif // CLAUSE_BASE_H
