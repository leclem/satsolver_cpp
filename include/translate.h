#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "enigme/latin.h"
#include "enigme/magic.h"
#include "enigme/sudoku.h"

#include "tseitin/tseitin.h"

#include "SMT/SMT.h"
#include "arith/arith.h"
#include "kcolor/kcolor.h"
// enumeration of the different possible translation

enum Translate {cnf, tseitin, latin, magic, sudoku, smt, arith, kcolor};


#endif // TRANSLATE_H
