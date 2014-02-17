#ifndef DPLL_SAT_H
#define DPLL_SAT_H

#include "base/sat.h"

// class to change nextVar behaviour
template<LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_nextVar<DPLL, _WAT, _CL> : virtual public CSAT_base_vars<DPLL, _WAT, _CL>
{
    typedef typename VariablePtr<DPLL, _WAT, _CL>::type VarPtr;

    using CSAT_base_vars<DPLL, _WAT, _CL>::m_unassignedVars;
    using CSAT_base_vars<DPLL, _WAT, _CL>::m_voidVar;

public:
    // next unassigned var
    CLiteral<DPLL, _WAT, _CL> nextLit()
    {
        if (!m_unassignedVars.empty())
            return CLiteral<DPLL, _WAT, _CL>(m_unassignedVars.front(), true);
        return CLiteral<DPLL, _WAT, _CL>(m_voidVar, true);
    }
};
#endif // DPLL_SAT_H
