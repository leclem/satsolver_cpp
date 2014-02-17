#ifndef MOMS_SAT_H
#define MOMS_SAT_H

#include <climits>

#include "base/sat.h"



// class to change nextVar behaviour
template<LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_nextVar<MOMS, _WAT, _CL> : virtual public CSAT_base_vars<MOMS, _WAT, _CL>
{
    using CSAT_base_vars<MOMS, _WAT, _CL>::m_assignedClauses;
    using CSAT_base_vars<MOMS, _WAT, _CL>::m_unassignedClauses;
    using CSAT_base_vars<MOMS, _WAT, _CL>::m_voidVar;
public:
    CSAT_base_nextVar()
    {
        m_isMinimumSizeDefined = false;
        m_minimumSize = INT_MAX;
    }
    virtual ~CSAT_base_nextVar(){}
    void addClauseToMOMSCounter(typename ClausePtr<MOMS, _WAT, _CL>::type);
    void rmClauseToMOMSCounter(typename ClausePtr<MOMS, _WAT, _CL>::type);
    void clearMOMSCounterOfZeros();

    int & getm_MinimumSize()
        { return m_minimumSize; }
    std::list< std::pair<int, CLiteral<MOMS, _WAT, _CL> > >  & getOccurenceOfEachLit()
        { return m_occurenceOfEachLit;}
    bool & getm_isMinimumSizeDefined()
        { return m_isMinimumSizeDefined; }
    // next unassigned var
    CLiteral<MOMS, _WAT, _CL> nextLit();
    protected:
    // variables just for MOMS problem
    bool m_isMinimumSizeDefined; // If minimumSize is calculated
    int m_minimumSize; //The minimum size of a clause
    void calculateMinimumSize();

    std::list< std::pair<int, CLiteral<MOMS, _WAT, _CL> > > m_occurenceOfEachLit;
};


/// ====================================== IMPLEMENTATION =======================================


// comparison for a list of pairs
template<LitChoice _WAT, ClauseChoice _CL>
bool compare_occurenceOfEachVar (std::pair<int,  CLiteral<MOMS, _WAT, _CL> > firstPair,
                                 std::pair<int,  CLiteral<MOMS, _WAT, _CL> > secondPair)
{
  return (firstPair.first > secondPair.first);
}

template<LitChoice _WAT, ClauseChoice _CL>
    CLiteral<MOMS, _WAT, _CL> CSAT_base_nextVar<MOMS, _WAT, _CL>::nextLit()
{
    if (!m_isMinimumSizeDefined){ //We construct the number of occurence of each variable for this minimum size
        m_occurenceOfEachLit.clear();
         //We first search the minimum size
        CSAT_base_nextVar<MOMS, _WAT, _CL>::calculateMinimumSize();

        //Now we count the occurences of each variable that appears

        for (typename ClausePtr<MOMS, _WAT, _CL>::type p_c = m_unassignedClauses.begin(); p_c!=m_unassignedClauses.end(); p_c++)
        {

            CSAT_base_nextVar<MOMS, _WAT, _CL>::addClauseToMOMSCounter(p_c);
        }
        //m_isMinimumSizeDefined = true;
        m_occurenceOfEachLit.sort(compare_occurenceOfEachVar<_WAT, _CL>);
    }
    if (m_occurenceOfEachLit.size() != 0)
    {

        CLiteral<MOMS, _WAT, _CL> litMaxi = m_occurenceOfEachLit.begin()->second;
        m_occurenceOfEachLit.pop_front();
        return litMaxi;
    }
    return CLiteral<MOMS, _WAT, _CL>(m_voidVar, true);
}

template<LitChoice _WAT, ClauseChoice _CL>
void CSAT_base_nextVar<MOMS, _WAT, _CL>::calculateMinimumSize()
{
    for(typename ClausePtr<MOMS, _WAT, _CL>::type p_c = m_unassignedClauses.begin(); p_c!=m_unassignedClauses.end(); p_c++)
        if ((int)(p_c->unassigneds().size()) < m_minimumSize)
            m_minimumSize = p_c->unassigneds().size();
}

template<LitChoice _WAT, ClauseChoice _CL>
void CSAT_base_nextVar<MOMS, _WAT, _CL>::addClauseToMOMSCounter(typename ClausePtr<MOMS, _WAT, _CL>::type p_c)
{
    bool foundVariable = false;
    //error
    for(typename std::list< CLiteral <MOMS, _WAT, _CL> >::iterator pVar = p_c->unassigneds().begin(); pVar != p_c->unassigneds().end(); pVar++)
    {
        foundVariable = false;
        //Then we  take this variable into account for our comparison
        for (typename std::list< std::pair<int, CLiteral<MOMS, _WAT, _CL> > >::iterator itOccOfEach = m_occurenceOfEachLit.begin(); itOccOfEach != m_occurenceOfEachLit.end(); itOccOfEach++)
        {
            CLiteral<MOMS, _WAT, _CL> curLiteral = *pVar;
            if (itOccOfEach->second == curLiteral)
            {
                (*itOccOfEach).first ++;
                foundVariable = true;
                break;
            }
        }
        if (!foundVariable)
        {
            //Then we have to add it at the list
            std::pair<int, CLiteral<MOMS, _WAT, _CL> > newLit;
            CLiteral<MOMS, _WAT, _CL> curLiteral = *pVar;
            newLit.second = curLiteral;
            newLit.first = 1;
            m_occurenceOfEachLit.push_back(newLit);
        }
    }
}

template<LitChoice _WAT, ClauseChoice _CL>
void CSAT_base_nextVar<MOMS, _WAT, _CL>::rmClauseToMOMSCounter(typename ClausePtr<MOMS, _WAT, _CL>::type p_c)
{
    for (typename CClause<MOMS, _WAT, _CL>::iterator pVar = p_c->begin(); pVar != p_c->end(); pVar++)
        {
        for (typename std::list< std::pair<int, CLiteral<MOMS, _WAT, _CL> > >::iterator itOccurenceOfEachLit =  m_occurenceOfEachLit.begin(); itOccurenceOfEachLit !=  m_occurenceOfEachLit.end(); itOccurenceOfEachLit++)
        {
            CLiteral<MOMS, _WAT, _CL> temp = *pVar;

            if (itOccurenceOfEachLit->second == temp.variable()){
                itOccurenceOfEachLit->first--;
            }
        }
    }
}

template<LitChoice _WAT, ClauseChoice _CL>
bool rmIfclearMOMSCounterOfZeros(std::pair<int, typename VariablePtr<MOMS, _WAT, _CL>::type > instance)
{
    return (instance.first <= 0 );
}

template<LitChoice _WAT, ClauseChoice _CL>
void CSAT_base_nextVar<MOMS, _WAT, _CL>::clearMOMSCounterOfZeros()
{
    m_occurenceOfEachLit.remove_if(rmIfclearMOMSCounterOfZeros);
}


#endif // MOMS_SAT_H
