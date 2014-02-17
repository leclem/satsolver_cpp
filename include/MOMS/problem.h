#ifndef MOMS_PROBLEM_H
#define MOMS_PROBLEM_H

#include "base/problem.h"

#include "MOMS/sat.h"

template<LitChoice _WAT, ClauseChoice _CL>
class CProblem_algo_base<MOMS, _WAT, _CL> : virtual public CProblem_base_static_vars<MOMS, _WAT, _CL>
{
using CProblem_base_static_vars<MOMS, _WAT, _CL>::static_sat;
protected:
    // initialisation of the recursive algortihm
    void initAlgo();

    // restore sat as before the solving pass
    void restoreAlgo();

    std::list< std::pair<int,typename VariablePtr<MOMS, _WAT, _CL>::type> > m_occurenceOfEachVarSave;
    int m_minimumSizeSave;
};

/// =========================== IMPLEMENTATION ========================

template<LitChoice _WAT, ClauseChoice _CL>
void CProblem_algo_base<MOMS, _WAT, _CL>::initAlgo()
{
  /*  m_occurenceOfEachVarSave = static_sat.getOccurenceOfEachVar();
    m_minimumSizeSave = static_sat.getm_isMinimumSizeDefined();*/
}

template<LitChoice _WAT, ClauseChoice _CL>
void CProblem_algo_base<MOMS, _WAT, _CL>::restoreAlgo()
{
    // restor MOMS var
   /* static_sat.getOccurenceOfEachVar() = m_occurenceOfEachVarSave;
    static_sat.getm_isMinimumSizeDefined() = m_minimumSizeSave;*/
}

#endif // MOMS_PROBLEM_H
