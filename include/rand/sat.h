#ifndef RAND_SAT_H
#define RAND_SAT_H

#include <cstdlib>

#include "base/sat.h"

// class to change nextVar behaviour
template<LitChoice _WAT, ClauseChoice _CL>
class CSAT_base_nextVar<RND, _WAT, _CL> : virtual public CSAT_base_vars<RND, _WAT, _CL>
{
    typedef typename VariablePtr<RND, _WAT, _CL>::type T_VarPtr;

    using CSAT_base_vars<RND, _WAT, _CL>::m_vars;
    using CSAT_base_vars<RND, _WAT, _CL>::m_voidVar;
    using CSAT_base_vars<RND, _WAT, _CL>::m_unassignedVars;

public:
    // next unassigned var
    CLiteral<RND, _WAT, _CL> nextLit()
    {
        int size = m_unassignedVars.size();
        if (size == 0){
            //we return the literal with equal probability true or false
            if ((rand() % 2) == 0){
                return CLiteral<RND, _WAT, _CL>(m_voidVar, true);
            }
            else{
                 return CLiteral<RND, _WAT, _CL>(m_voidVar, false);
            }
        }
        else{
            int randNumberVar = rand() % (m_unassignedVars.size()-1);
            return CLiteral<RND, _WAT, _CL>(m_unassignedVars[randNumberVar], true);
        }
    }
};

#endif // SAT_H_INCLUDED
