#ifndef LITERAL_BASE_H
#define LITERAL_BASE_H

#include <iostream>

#include "base/container.h"

// class for literals, containing vars and logic value
// the template parameter is the type of included var

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
class CLiteral
{
public:
    // constructor
    CLiteral() : m_logicValue(true){}
    CLiteral(typename VariablePtr<_ALG, _WAT, _CL>::type _var, bool _lv)
        : m_var(_var), m_logicValue(_lv) {}

    // reference of the contained variable
    typename VariablePtr<_ALG, _WAT, _CL>::type& variable() { return m_var;}

    // reference of the logic value
    bool& logicValue() {return m_logicValue;}

    // negate the literal
    CLiteral operator !() const {return CLiteral(m_var, !m_logicValue);}

    // literal comparaisons (for sort, ...)
    bool operator < (const CLiteral& _l) const
    {
        CLiteral __l = _l;
        if (__l.variable()->name() == m_var->name())
        {
            if(__l.logicValue() == m_logicValue)
                return false;
            else
                return m_logicValue;
        }
        else
            return __l.variable()->name() < m_var->name();
    }
    bool operator == (const CLiteral& _l) const
    {
        CLiteral lit = _l;
        return (lit.variable() == m_var && lit.logicValue() == m_logicValue);
    }
private:
    typename VariablePtr<_ALG, _WAT, _CL>::type m_var; // variable of the literal
    bool m_logicValue;        // logic value
};

// output for the literal
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    std::ostream& operator << (std::ostream& out, CLiteral<_ALG, _WAT, _CL> l)
{
    out << ((l.logicValue())?"":"!") << l.variable()->name();
    return out;
}

#endif // LITERAL_BASE_H
