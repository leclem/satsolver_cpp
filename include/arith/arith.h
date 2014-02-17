#ifndef ARITH_ARITH_H
#define ARITH_ARITH_H

#include <iostream>

class CArith
{
public:
    static void translateIn(std::istream& in, std::ostream& result);
    static void translateOut(std::istream& out, std::ostream& result);
private:
    static int num_eq; // number of equations
};

#endif // ARITH_ARITH_H
