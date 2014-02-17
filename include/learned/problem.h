#ifndef LEARNED_PROBLEM_H
#define LEARNED_PROBLEM_H

#include "base/problem.h"

#include "conflict.h"

template<SATheuristic _ALG, LitChoice _WAT>
class CProblem_conflict<_ALG, _WAT, LEARNED> : virtual public CProblem_base_static_vars<_ALG, _WAT, LEARNED>
{
    typedef CClause<_ALG, _WAT, LEARNED> Clause;
    typedef typename ClausePtr<_ALG, _WAT, LEARNED>::type ClausePtr;
    typedef CLiteral<_ALG, _WAT, LEARNED> Literal;
    typedef typename std::list<Literal>::iterator LitIter;
protected:
    using CProblem_base_static_vars<_ALG, _WAT, LEARNED>::static_sat;
public:
    virtual ~CProblem_conflict() {}
    // update the variable of the conflict
    void updateProblem()
    {
        static_level++;
    }
    void restoreProblem()
    {
        static_level--;
    }
    // say if we should continue to backtrack or if we must continue
    bool mustBacktrack()
    {
        return static_level>static_backtrack_level;
    }
    // set a deduced literal
    void setDeducedLit(Literal l, ClausePtr c)
    {
        if (l==bet)
            return;
        l.variable()->deducClause() = c;
        l.variable()->bet() = false;
        l.variable()->level() = static_level;
        l.variable()->graphDescr() = 0;
    }
    // set a bet literal
    void setBetLit(Literal l)
    {
        l.variable()->bet() = true;
        l.variable()->level() = static_level;
        l.variable()->graphDescr() = 0;
        bet = l;
    }
    // handle the conflict
    void handleConflict();
protected:
    static unsigned static_level;
    static unsigned static_backtrack_level;
    Literal bet;
};

/// ===================================== implementation =====================================

template<SATheuristic _ALG, LitChoice _WAT>
void CProblem_conflict<_ALG, _WAT, LEARNED>::handleConflict()
{
    // conflict before the thirst bet is not really a conflict ...
    if(!static_level)
        return;

    // clause that create the conflict
    ClausePtr conflict = static_sat.getConflictClause();

    // clause learned
    Clause learned;

    // simple path from conflict to bet
    std::list<Literal> path;

    // current clause of path
    ClausePtr cl = conflict;

    // curent depth of the path
    int curentDepth = 0;

    while(true)
    {
        // parse all vars and get the first that is of the same level
        LitIter it;
        // update clauses for watched, yes, they are not so useful together ...
        cl->update();
        for(it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
            if(it->variable()->level() == static_level)
                break;

        // say we found the variable
        it->variable()->graphDescr() = ++curentDepth;


        //update the path
        path.push_back(*it);

        // exit if we found the bet
        if(it->variable()->bet())
            break;

        // change current clause
        cl = it->variable()->deducClause();
    }

    // greatest path member found
    int greatPath = 0;

    //clauses to parse
    std::list<ClausePtr> parse;
    parse.push_back(conflict);

    // current variable of the path
    typename std::list<Literal>::iterator curLit = path.begin();

    static_backtrack_level = 0;

    // UIP
    Literal UIP;

    //parse the graph from each node of the path, until we arrive at the greatest member found
    while(!parse.empty())
    {
        cl = parse.front();
        parse.pop_front();
        cl->update();
        for(LitIter it = cl->assigneds(false).begin(); it != cl->assigneds(false).end(); it++)
            if(it->variable()->level() == static_level)
            {
                // if we are deeper in the past, we remember it
                if(it->variable()->graphDescr() > greatPath)
                    greatPath = it->variable()->graphDescr();
                // else if we didin't already see that variable, we add the corresponding clause, and set it to seen
                else if(!it->variable()->graphDescr())
                {
                    parse.push_back(it->variable()->deducClause());
                    it->variable()->graphDescr() = -1;
                }
            }
            else // we found litteral is of another level, we can learn it
            {
                static_backtrack_level = std::max(static_backtrack_level, it->variable()->level());
                learned.addLiteral(*it);
            }


        if(parse.empty())
        {
            // we found the node
            if(curLit->variable()->graphDescr() == greatPath)
            {
                UIP = *curLit;
                learned.addLiteral(*curLit);
            }
            else
                parse.push_back((curLit++)->variable()->deducClause());
        }
    }

    if(learned.assigneds(true).empty() && learned.assigneds(false).empty() && learned.unassigneds().empty())
        static_backtrack_level = static_level;
    else
    {
        ClausePtr p_c = static_sat.addClause(learned);
        CConflict<_ALG, _WAT, LEARNED>::conflictManager(static_sat, p_c, UIP, static_level);
    }
}

template<SATheuristic _ALG, LitChoice _WAT>
unsigned CProblem_conflict<_ALG, _WAT, LEARNED>::static_level = 0;

template<SATheuristic _ALG, LitChoice _WAT>
unsigned CProblem_conflict<_ALG, _WAT, LEARNED>::static_backtrack_level = 0;

#endif // LEARNED_PROBLEM_H
