#ifndef CHOICE_H
#define CHOICE_H

// enumeration to choose between classic and watched literals
enum LitChoice {CLASSIC, WATCHED};

// enumeration to choose the sat algo
enum SATheuristic {DPLL, RND, MOMS, DLIS};

// enumeration to choose between standard and learned clauses
enum ClauseChoice {STANDART, LEARNED};

#define SETCONFLICTARGS(__w_var, __c_var, __ALGO, bInteractive, bDoc)                       \
if((__c_var)==STANDART)                                             \
{                                                                   \
    if((__w_var)==CLASSIC)                                          \
        CConflict<__ALGO, CLASSIC, STANDART>::setArgs(bInteractive, bDoc);\
    else                                                            \
        CConflict<__ALGO, WATCHED, STANDART>::setArgs(bInteractive, bDoc);\
}                                                                   \
else                                                                \
{                                                                   \
    if((__w_var)==CLASSIC)                                          \
        CConflict<__ALGO, CLASSIC, LEARNED>::setArgs(bInteractive, bDoc);\
    else                                                            \
        CConflict<__ALGO, WATCHED, LEARNED>::setArgs(bInteractive, bDoc);\
}

// define to help make the choice shorter
#define SOLVE(__w_var, __c_var, __ALGO, __in, __out)                \
if((__c_var)==STANDART)                                             \
{                                                                   \
    if((__w_var)==CLASSIC)                                          \
        CProblem<__ALGO, CLASSIC, STANDART>::solve((__in), (__out));\
    else                                                            \
        CProblem<__ALGO, WATCHED, STANDART>::solve((__in), (__out));\
}                                                                   \
else                                                                \
{                                                                   \
    if((__w_var)==CLASSIC)                                          \
        CProblem<__ALGO, CLASSIC, LEARNED>::solve((__in), (__out)); \
    else                                                            \
        CProblem<__ALGO, WATCHED, LEARNED>::solve((__in), (__out)); \
}



#endif // CHOICE_H
