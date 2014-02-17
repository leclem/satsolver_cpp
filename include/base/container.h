#ifndef CONTAINER_BASE_H
#define CONTAINER_BASE_H

#include <list>

#include "choice.h"

// define for sending an error when unspecialized function is called
#ifdef _DEBUG
    #include <iostream>
    #define MUST_SPECIALIZE std::cerr << "ERROR, call of unspecialized function. Base in" <<__FILE__ << "; line " << __LINE__ <<std::endl
#else
    #define MUST_SPECIALIZE
#endif


// declaration for variables and clauses
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    class CVariable;

template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    class CClause;

// base data structures and pointers for clauses and vars containers

// SAT clause containers
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    struct SATContener
        { typedef std::list<CClause<_ALG, _WAT, _CL> > type; };

// clause pointer
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    struct ClausePtr
        { typedef typename SATContener<_ALG, _WAT, _CL>::type::iterator type; };


// list of clause ptrs
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    struct ClausePtrList
        { typedef std::list<typename ClausePtr<_ALG, _WAT, _CL>::type> type; };


// list of vars
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    struct VarList
        { typedef std::list<CVariable<_ALG, _WAT, _CL> > type; };

// var pointer
template<SATheuristic _ALG, LitChoice _WAT, ClauseChoice _CL>
    struct VariablePtr
        { typedef typename VarList<_ALG, _WAT, _CL>::type::iterator type; };

#endif // CONTAINER_BASE_H
