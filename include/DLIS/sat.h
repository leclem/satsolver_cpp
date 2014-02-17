#ifndef DLIS_PROBLEM_H
#define DLIS_PROBLEM_H

#include "base/sat.h"

// class to change nextVar behaviour
template<LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_nextVar<DLIS, _WAT, _CL> : virtual public CSAT_base_vars<DLIS, _WAT, _CL>
{
    typedef typename std::vector<typename VariablePtr<DLIS, _WAT, _CL>::type>::iterator VarPtrIt;
    using CSAT_base_vars<DLIS, _WAT, _CL>::m_voidVar;
    using CSAT_base_vars<DLIS, _WAT, _CL>::m_unassignedVars;

public:
    // next unassigned var
    CLiteral<DLIS, _WAT, _CL> nextLit()
    {
        unsigned occurence  = 0;
        CLiteral<DLIS, _WAT, _CL> cur_lit(m_voidVar, true);

        for(VarPtrIt var_it = m_unassignedVars.begin(); var_it != m_unassignedVars.end(); var_it++)
        {
            if(occurence < (*var_it)->numUnassignedClauses(true))
            {
                cur_lit = CLiteral<DLIS, _WAT, _CL>(*var_it, true);
                occurence = (*var_it)->numUnassignedClauses(true);
            }
            if(occurence < (*var_it)->numUnassignedClauses(false))
            {
                cur_lit = CLiteral<DLIS, _WAT, _CL>(*var_it, false);
                occurence = (*var_it)->numUnassignedClauses(false);
            }
        }
        return cur_lit;
    }
};
#endif // DLIS_SAT_H
