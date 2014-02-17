#ifndef LEARNED_VARIABLE_H
#define LEARNED_VARIABLE_H

#include "base/variable.h"
#include "base/clause.h"

template<SATheuristic _ALG, LitChoice _WAT>
class CVariable_learnedBase<_ALG, _WAT, LEARNED>
{
public:
    unsigned& level() {return m_uLevel;}
    typename ClausePtr<_ALG, _WAT, LEARNED>::type& deducClause() {return m_deducingClause; }
    bool& bet() {return m_bet;}
    int& graphDescr() {return m_graphDescr;}
protected:
    unsigned m_uLevel;
    typename ClausePtr<_ALG, _WAT, LEARNED>::type m_deducingClause;
    bool m_bet;
    int m_graphDescr;
};

#endif // LEARNED_LITERALS_H
