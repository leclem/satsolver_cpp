#ifndef VARIABLES_BASE_H
#define VARIABLES_BASE_H

#include <string>
#include <sstream>
#include <cmath>

#include <iostream>

#include "base/container.h"

// struct to store the informations about the clauses
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
struct CClauseInfo
{
    CClauseInfo (typename ClausePtr<_ALG, _WAT, _CL>::type _c, typename std::list<CLiteral<_ALG, _WAT, _CL> >::iterator _p)
        : clause(_c), pos(_p) {}
    // ptr to the clause
    typename ClausePtr<_ALG, _WAT, _CL>::type clause;

    // positions of the variable in the clause
    typename std::list<CLiteral<_ALG, _WAT, _CL> >::iterator pos;
};

// class containing vars, that can be herited from CVariable
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    class CVariable_base
{};

// class containing vars, that can be herited from CVariable
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    class CVariable_learnedBase
{};

// class base for variables
// template parameter : ptr to the clauses that caintains these vars
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    class CVariable : public CVariable_base<_ALG, _WAT, _CL>, public CVariable_learnedBase<_ALG, _WAT, _CL>
{
public:

    // clause info type
    typedef CClauseInfo<_ALG, _WAT, _CL> ClInfo;
    typedef std::list<ClInfo> ClInfoList;

    // default constructor
    CVariable()
        : m_isAssigned(false)
        { setDefaultName(); }

    // construction with an assignation
    CVariable(bool assignement)
        { assigne(assignement); setDefaultName(); }

    // construction with a name
    CVariable(std::string _name)
        :m_name_private(_name), m_isAssigned(false)
        {}

    // construction with a name and an assignment
    CVariable(std::string _name, bool assignement)
        : m_name_private(_name)
        { assigne(assignement); }

    // destructor
    virtual ~CVariable() {}

    //Tells us if we already assigned the variable
    bool isAssigned()
        { return m_isAssigned; }

    // Tells us the assignation (true or false) of the variable
    bool assignement()
    {
        if (m_isAssigned)
            return m_assignement;
        // else wa have an error ...
        std::cerr << "Erreur : demande de l'assignement d'une variable non assignée.";
        if (!this->name().empty()){
                std::cerr << " (Variable : " << this->name() << ')';
        }
        std::cerr << std::endl;
        return false;//We return some value
    }

    // assign the variable
    void assigne(bool assignement)
        { m_assignement = assignement; m_isAssigned = true; }

    // unassign the variable
    void unassign()
        { m_isAssigned = false; }

    // get the clauses with the corresponding assignement
    ClInfoList& getClauses(bool assignement)
        { return (assignement)? m_clausesPositive : m_clausesNegative;}

    unsigned& numUnassignedClauses(bool a)
        { return (a) ? m_trueUnassClauses : m_falseUnassClauses; }

    // Adds a ptr to a clause that contains this variable.
    // The boolean specifies if the variable is negated or not is the clause (false = negated)
    void addClauseInfo(ClInfo pClause, bool sign)
    {
        getClauses(sign).push_back(pClause);

        if(!pClause.clause->isVerified())
            numUnassignedClauses(sign)++;
    }

    // get a reference to the name of the clause
    std::string& name() { return m_name_private; }

    size_t& assignmentPos()
        { return m_posInAssignmentVector; }

protected:

    // give a default name for the variable, so that each var has a different name
    void setDefaultName()
    {
        static int count = 0;
        std::stringstream _name;
        _name << "__unamed" << count++;
        _name >> m_name_private;
    }

    ClInfoList m_clausesPositive;
    ClInfoList m_clausesNegative;

    std::string m_name_private;

    bool m_isAssigned;
    bool m_assignement;

    size_t m_posInAssignmentVector; // position dans le vecteur d'assignement de sat

    // nombre de clauses non assignées
    unsigned m_trueUnassClauses;
    unsigned m_falseUnassClauses;
};

#endif // VARIABLES_BASE_H
