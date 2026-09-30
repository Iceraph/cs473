#ifndef MYFLPT_CONST_H
#define MYFLPT_CONST_H
#include "myflpt_lib.h"   // needs BIAS, FLPT_MANTISSE, SIGN_MASK, MANTISSE_MASK

//! Compile-time float -> flpt_t conversion, usable in global initializers.
//! Works for |x| in [2^-30, 2^13); x must be a constant expression. Truncates extra bits.
#define FLPT__ABS(x) ((x) < 0 ? -(x) : (x))
#define FLPT__EXP(a) ( \
    (a) >= 0x1p+12 ? 12 : \
    (a) >= 0x1p+11 ? 11 : \
    (a) >= 0x1p+10 ? 10 : \
    (a) >= 0x1p+9 ? 9 : \
    (a) >= 0x1p+8 ? 8 : \
    (a) >= 0x1p+7 ? 7 : \
    (a) >= 0x1p+6 ? 6 : \
    (a) >= 0x1p+5 ? 5 : \
    (a) >= 0x1p+4 ? 4 : \
    (a) >= 0x1p+3 ? 3 : \
    (a) >= 0x1p+2 ? 2 : \
    (a) >= 0x1p+1 ? 1 : \
    (a) >= 0x1p+0 ? 0 : \
    (a) >= 0x1p-1 ? -1 : \
    (a) >= 0x1p-2 ? -2 : \
    (a) >= 0x1p-3 ? -3 : \
    (a) >= 0x1p-4 ? -4 : \
    (a) >= 0x1p-5 ? -5 : \
    (a) >= 0x1p-6 ? -6 : \
    (a) >= 0x1p-7 ? -7 : \
    (a) >= 0x1p-8 ? -8 : \
    (a) >= 0x1p-9 ? -9 : \
    (a) >= 0x1p-10 ? -10 : \
    (a) >= 0x1p-11 ? -11 : \
    (a) >= 0x1p-12 ? -12 : \
    (a) >= 0x1p-13 ? -13 : \
    (a) >= 0x1p-14 ? -14 : \
    (a) >= 0x1p-15 ? -15 : \
    (a) >= 0x1p-16 ? -16 : \
    (a) >= 0x1p-17 ? -17 : \
    (a) >= 0x1p-18 ? -18 : \
    (a) >= 0x1p-19 ? -19 : \
    (a) >= 0x1p-20 ? -20 : \
    (a) >= 0x1p-21 ? -21 : \
    (a) >= 0x1p-22 ? -22 : \
    (a) >= 0x1p-23 ? -23 : \
    (a) >= 0x1p-24 ? -24 : \
    (a) >= 0x1p-25 ? -25 : \
    (a) >= 0x1p-26 ? -26 : \
    (a) >= 0x1p-27 ? -27 : \
    (a) >= 0x1p-28 ? -28 : \
    (a) >= 0x1p-29 ? -29 : \
    (a) >= 0x1p-30 ? -30 : \
    -999)
#define FLPT__SCL(a) ( \
    (a) >= 0x1p+12 ? 0x1p-12 : \
    (a) >= 0x1p+11 ? 0x1p-11 : \
    (a) >= 0x1p+10 ? 0x1p-10 : \
    (a) >= 0x1p+9 ? 0x1p-9 : \
    (a) >= 0x1p+8 ? 0x1p-8 : \
    (a) >= 0x1p+7 ? 0x1p-7 : \
    (a) >= 0x1p+6 ? 0x1p-6 : \
    (a) >= 0x1p+5 ? 0x1p-5 : \
    (a) >= 0x1p+4 ? 0x1p-4 : \
    (a) >= 0x1p+3 ? 0x1p-3 : \
    (a) >= 0x1p+2 ? 0x1p-2 : \
    (a) >= 0x1p+1 ? 0x1p-1 : \
    (a) >= 0x1p+0 ? 0x1p+0 : \
    (a) >= 0x1p-1 ? 0x1p+1 : \
    (a) >= 0x1p-2 ? 0x1p+2 : \
    (a) >= 0x1p-3 ? 0x1p+3 : \
    (a) >= 0x1p-4 ? 0x1p+4 : \
    (a) >= 0x1p-5 ? 0x1p+5 : \
    (a) >= 0x1p-6 ? 0x1p+6 : \
    (a) >= 0x1p-7 ? 0x1p+7 : \
    (a) >= 0x1p-8 ? 0x1p+8 : \
    (a) >= 0x1p-9 ? 0x1p+9 : \
    (a) >= 0x1p-10 ? 0x1p+10 : \
    (a) >= 0x1p-11 ? 0x1p+11 : \
    (a) >= 0x1p-12 ? 0x1p+12 : \
    (a) >= 0x1p-13 ? 0x1p+13 : \
    (a) >= 0x1p-14 ? 0x1p+14 : \
    (a) >= 0x1p-15 ? 0x1p+15 : \
    (a) >= 0x1p-16 ? 0x1p+16 : \
    (a) >= 0x1p-17 ? 0x1p+17 : \
    (a) >= 0x1p-18 ? 0x1p+18 : \
    (a) >= 0x1p-19 ? 0x1p+19 : \
    (a) >= 0x1p-20 ? 0x1p+20 : \
    (a) >= 0x1p-21 ? 0x1p+21 : \
    (a) >= 0x1p-22 ? 0x1p+22 : \
    (a) >= 0x1p-23 ? 0x1p+23 : \
    (a) >= 0x1p-24 ? 0x1p+24 : \
    (a) >= 0x1p-25 ? 0x1p+25 : \
    (a) >= 0x1p-26 ? 0x1p+26 : \
    (a) >= 0x1p-27 ? 0x1p+27 : \
    (a) >= 0x1p-28 ? 0x1p+28 : \
    (a) >= 0x1p-29 ? 0x1p+29 : \
    (a) >= 0x1p-30 ? 0x1p+30 : \
    0.0)

//! \brief Compile-time float -> flpt_t conversion, usable in global initializers.
//! Generated code by Claude OPUS 5.5 
#define FLPT_CONST(x) ((flpt_t)( ((x) == 0 || FLPT__EXP(FLPT__ABS(x)) + (int)(BIAS >> FLPT_MANTISSE) <= 0) ? 0u : \
    ( ((x) < 0 ? SIGN_MASK : 0u) \
    | ((uint32_t)(FLPT__EXP(FLPT__ABS(x)) + (int)(BIAS >> FLPT_MANTISSE)) << FLPT_MANTISSE) \
    | ((uint32_t)((FLPT__ABS(x) * FLPT__SCL(FLPT__ABS(x)) - 1.0) * (double)(1u << FLPT_MANTISSE)) & MANTISSE_MASK) )))

#endif // MYFLPT_CONST_H
