#ifndef FLPT_LIB_H
#define FLPT_LIB_H
#include "flpt.h"

// ------------ Constants -------------
// Dynamic masks depending on the number of bits for exponent, sign and mantisse
#define EXPONENT_MASK ((0xFFFFFFFF << (FLPT_MANTISSE + FLPT_SIGN)) >> FLPT_SIGN)
#define MANTISSE_MASK (0xFFFFFFFF >> (FLPT_SIGN + FLPT_EXPONENT))
#define NEG_MANTISSE_MASK (~MANTISSE_MASK)
#define MANTISSE_OVERFLOW_MASK (1u << (FLPT_MANTISSE + 1)) // Overflow bit with hidden bit
#define MANTISSE_HIDDEN_BIT_MASK (1u << FLPT_MANTISSE) // Number is 1.mantisse
#define MANTISSE_LOW_MASK (0xFFFFFFFF >> 16) // Low 16 bits of mantisse
#define NOT_MANTISSE_HIDDEN_BIT_MASK (~MANTISSE_HIDDEN_BIT_MASK) // Mask to remove hidden bit from mantisse
#define SIGN_MASK (1u << (FLPT_MANTISSE + FLPT_EXPONENT))
#define NEG_SIGN_MASK (~SIGN_MASK)

#define BIAS (26 << FLPT_MANTISSE) // 2^(FLPT_EXPONENT-1) - 6 (max value should be (4+cx)^2 + cy^2 ≤ 43)

#define MANT_SHIFT (FLPT_MANTISSE - 16 + 1) // shift mantisse down to 16 bits to prevent overflow
#define SHIFT_EXP_OVERFLOW (FLPT_EXPONENT - 1)

// ------------ Prototype of helper functions -------------
static inline flpt_t flpt_64_mant_mul(uint32_t a_mant, uint32_t b_mant);
static inline flpt_t flpt_32_mant_mul(uint32_t *a, uint32_t *b);

// ------------ Implementation of functions -------------

//! \brief Double the flpt point number
static inline flpt_t flpt_double (const flpt_t a) {
    // If exp + mantisse is 0, return 0. Otherwise, add 1 to the exponent
    if ((a & NEG_SIGN_MASK) == 0) {
        return 0;
    }
    return (a + (MANTISSE_HIDDEN_BIT_MASK)); // exp + 1);
}

//! \brief Add two flpt point numbers
//! Handles the different cases and redirects to the appropriate function for addition
static inline flpt_t flpt_add(const flpt_t a, const flpt_t b) {
    uint32_t mag_a = a & NEG_SIGN_MASK; 
    uint32_t mag_b = b & NEG_SIGN_MASK;
    int swap   = mag_a < mag_b;
    flpt_t high  = swap ? b : a;
    flpt_t low  = swap ? a : b;
    uint32_t mag_h = high & NEG_SIGN_MASK;  // recompute magnitudes instead of swapping
    uint32_t mag_l = low & NEG_SIGN_MASK;
    if (mag_l == 0) {
        return high;
    }
    uint32_t shift = (mag_h >> FLPT_MANTISSE) - (mag_l >> FLPT_MANTISSE);
    if (shift > FLPT_MANTISSE) {
        return high;
    }
    uint32_t mant_h = (high & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK;
    uint32_t mant_l = ((low & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> shift;
    flpt_t result = high & NEG_MANTISSE_MASK;   // sign + exponent
    if ((high ^ low) & SIGN_MASK) { // different signs
        uint32_t result_mant = mant_h - mant_l;
        if (result_mant == 0) return 0;
        uint32_t k = FLPT_MANTISSE + 1 - fl1(result_mant);
        result -= k << FLPT_MANTISSE;
        result_mant <<= k;
        return result + result_mant - MANTISSE_HIDDEN_BIT_MASK;
    } else {// same sign
        uint32_t result_mant = mant_h + mant_l;
        uint32_t n = result_mant >> (FLPT_MANTISSE + 1);
        result_mant >>= n;
        return result + (n << FLPT_MANTISSE) + result_mant - MANTISSE_HIDDEN_BIT_MASK;
    }
}

//! \brief Subtract two flpt point numbers
// Equivalent to flpt_add(a, b^sign)
static inline flpt_t flpt_sub(const flpt_t a, const flpt_t b) {
    uint32_t mag_a = a & NEG_SIGN_MASK; 
    uint32_t mag_b = b & NEG_SIGN_MASK;
    int swap   = mag_a < mag_b;
    flpt_t high  = swap ? b : a;
    flpt_t low  = swap ? a : b;
    uint32_t mag_h = high & NEG_SIGN_MASK;  // recompute magnitudes instead of swapping
    uint32_t mag_l = low & NEG_SIGN_MASK;
    if (mag_l == 0) {
        return high;
    }
    uint32_t shift = (mag_h >> FLPT_MANTISSE) - (mag_l >> FLPT_MANTISSE);
    if (shift > FLPT_MANTISSE) {
        return high;
    }
    uint32_t mant_h = (high & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK;
    uint32_t mant_l = ((low & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> shift;
    flpt_t result = high & NEG_MANTISSE_MASK;   // sign + exponent
    if ((high ^ low) & SIGN_MASK) { // different signs
        uint32_t result_mant = mant_h + mant_l;
        uint32_t n = result_mant >> (FLPT_MANTISSE + 1);
        result_mant >>= n;
        return result + (n << FLPT_MANTISSE) + result_mant - MANTISSE_HIDDEN_BIT_MASK;
    } else {// same sign
        uint32_t result_mant = mant_h - mant_l;
        if (result_mant == 0) return 0;
        uint32_t k = FLPT_MANTISSE + 1 - fl1(result_mant);
        result -= k << FLPT_MANTISSE;
        result_mant <<= k;
        return result + result_mant - MANTISSE_HIDDEN_BIT_MASK;
    }
}

//! \brief Multiply two flpt point numbers
static inline flpt_t flpt_mul(const flpt_t a, const flpt_t b) {
    int32_t exp_a = (a & EXPONENT_MASK);
    int32_t exp_b = (b & EXPONENT_MASK);
    int32_t exp_result = (exp_a) + (exp_b) - BIAS; // overflow shouldn't be possible
    // If any of the exponents is 0 or negative, return 0. If the result exponent is negative, return 0.
    if (((exp_a - (int32_t)MANTISSE_HIDDEN_BIT_MASK) | (exp_b - (int32_t)MANTISSE_HIDDEN_BIT_MASK) |
        (exp_result - (int32_t)MANTISSE_HIDDEN_BIT_MASK)) < 0) {
    
        return 0;
    }

    flpt_t result = exp_result;
    
    uint32_t a_mant = (((a & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK)) << FLPT_EXPONENT; // 32 bits mantisse with hidden bit
    uint32_t b_mant = (((b & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK)) << FLPT_EXPONENT;
    uint32_t result_mant = (flpt_64_mant_mul(a_mant, b_mant) >> (SHIFT_EXP_OVERFLOW));

    if (result_mant & MANTISSE_OVERFLOW_MASK) {
        result_mant >>= 1; // shift mantisse down
        result += (MANTISSE_HIDDEN_BIT_MASK); // add 1 to the exponent 
    }
    result |= (result_mant & NOT_MANTISSE_HIDDEN_BIT_MASK); // Remove hidden bit and add to result
    result |= (a ^ b) & (SIGN_MASK);
    return result;
}

//! \brief Dedicated function to square a flpt point number
static inline flpt_t flpt_square(const flpt_t a) {
    flpt_t result = 0;
    int32_t exp_a = (a & EXPONENT_MASK);
    int32_t exp_result = (exp_a)+ (exp_a) - BIAS; // overflow shouldn't be possible
    if (((exp_a - (int32_t)MANTISSE_HIDDEN_BIT_MASK) |
        (exp_result - (int32_t)MANTISSE_HIDDEN_BIT_MASK)) < 0) {
        
        return 0;
    }

    result |= exp_result;
    
    uint32_t a_mant = (((a & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK)) << FLPT_EXPONENT;
    uint32_t a_high = a_mant >> 16;
    uint32_t a_low = (a_mant & MANTISSE_LOW_MASK);
    uint32_t result_mant = (a_high * a_high + ((a_high * a_low) >> 15)); // ignore the low*low part
    result_mant >>= (FLPT_EXPONENT - 1); // shift mantisse down to 32 bits
    if (result_mant & MANTISSE_OVERFLOW_MASK) {
        result_mant >>= 1; // shift mantisse down
        result += (MANTISSE_HIDDEN_BIT_MASK); // add 1 to the exponent 
    }
    result |= (result_mant & NOT_MANTISSE_HIDDEN_BIT_MASK); // Remove hidden bit and add to result
    return result;
}


// ------------ Helper functions -------------
//! \brief Multiply two 32-bit mantisse numbers with hi & lo and return the 32-bit result
static inline flpt_t flpt_64_mant_mul(uint32_t a_mant, uint32_t b_mant) {
    uint32_t a_high = a_mant >> 16;
    uint32_t b_high = b_mant >> 16;
    uint32_t a_low = (a_mant & MANTISSE_LOW_MASK);
    uint32_t b_low = (b_mant & MANTISSE_LOW_MASK);

    return (a_high * b_high + ((a_high * b_low) >> 16) + ((a_low * b_high) >> 16)); // ignore the low*low part
}

//! \brief Multiply two 32-bit mantisse numbers with loss in precision and return the 32-bit result
static inline flpt_t flpt_32_mant_mul(uint32_t *a, uint32_t *b) {
    uint32_t a_mant = ((*a & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> MANT_SHIFT; // need to implement proper way
    uint32_t b_mant = ((*b & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> MANT_SHIFT; // without losing precision
    return((a_mant * b_mant) >> (FLPT_EXPONENT));
}

//! \brief Calculate the position of the first leading 1 in a 32-bit number
static inline uint32_t fl1(uint32_t x) {
    uint32_t r;
    __asm__("l.fl1 %0,%1" : "=r"(r) : "r"(x));
    return r;
}

#endif // FLPT_LIB_H
