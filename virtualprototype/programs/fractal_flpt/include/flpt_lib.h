#ifndef FLPT_LIB_H
#define FLPT_LIB_H
#include "flpt.h"

#define EXPONENT_MASK ((0xFFFFFFFF << (FLPT_MANTISSE + FLPT_SIGN)) >> FLPT_SIGN)
#define MANTISSE_MASK (0xFFFFFFFF >> (FLPT_SIGN + FLPT_EXPONENT))
#define MANTISSE_OVERFLOW_MASK (1u << (FLPT_MANTISSE + 1)) // Overflow bit with hidden bit
#define MANTISSE_HIDDEN_BIT_MASK (1u << FLPT_MANTISSE) // Number is 1.mantisse
#define MANTISSE_LOW_MASK (0xFFFFFFFF >> 16) // Low 16 bits of mantisse
#define NOT_MANTISSE_HIDDEN_BIT_MASK (~MANTISSE_HIDDEN_BIT_MASK)
#define SIGN_MASK (1u << (FLPT_MANTISSE + FLPT_EXPONENT))
#define NEG_SIGN_MASK (~SIGN_MASK)

#define BIAS (26 << FLPT_MANTISSE) // 2^(FLPT_EXPONENT-1) - 6 (max value should be (4+cx)^2 + cy^2 ≤ 43)

#define MANT_SHIFT (FLPT_MANTISSE - 16 + 1) // shift mantisse down to 16 bits to prevent overflow

// ------------ Prototype of helper functions -------------
static inline flpt_t flpt_64_mant_mul(uint32_t a_mant, uint32_t b_mant);
static inline flpt_t flpt_32_mant_mul(uint32_t *a, uint32_t *b);

// ------------ Implementation of functions -------------

static inline flpt_t flpt_double (const flpt_t a) {
    if ((a & NEG_SIGN_MASK) == 0) {
        return 0;
    }
    return (a + (MANTISSE_HIDDEN_BIT_MASK)); // exp + 1);
}

static inline flpt_t flpt_sub (const flpt_t a, const flpt_t b) {
    return flpt_add(a, (b ^SIGN_MASK));
}

//! \brief Add two flpt point numbers
//! Handles the different cases and redirects to the appropriate function for addition
static inline flpt_t flpt_add(const flpt_t a, const flpt_t b) {
    uint32_t unsigned_a = a & NEG_SIGN_MASK;
    uint32_t unsigned_b = b & NEG_SIGN_MASK;

    if ((a ^ b) & SIGN_MASK) { // Different signs, subtract the numbers
        if (unsigned_a > unsigned_b) {
            if (unsigned_b == 0) return a;
            return flpt_sub_raw(a, b);
        } if (unsigned_a < unsigned_b) {
            if (unsigned_a == 0) return b;
            return flpt_sub_raw(b, a);
        }
    } else { // Same signs,  add the numbers
        if (unsigned_a > unsigned_b) {
            if (unsigned_b == 0) return a;
            return flpt_add_raw(a, b);
        } if (unsigned_a < unsigned_b) {
            if (unsigned_a == 0) return b;
            return flpt_add_raw(b, a);
        }

    }

    if (a == b) {
        return flpt_double(a);
    }
    return 0;
}


//! Knowing the high and low numbers, add them together and return the result
static inline flpt_t flpt_add_raw(const flpt_t high, const flpt_t low) {
    uint32_t shift = ((high & EXPONENT_MASK) - (low & EXPONENT_MASK)) >> FLPT_MANTISSE;
    if (shift > FLPT_MANTISSE) {
        return high; // low number is too small to affect the result
    }
    flpt_t result = (high & SIGN_MASK) | (high & EXPONENT_MASK);
    // Shift mantisse of low number
    uint32_t result_mant = ((low & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> shift;
    // Add mantisse of high number
    result_mant += ((high & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK);
    // Handle overflow of mantisse
    if (result_mant & MANTISSE_OVERFLOW_MASK) {
        result_mant >>= 1; // shift mantisse down
        result += (MANTISSE_HIDDEN_BIT_MASK); // add 1 to the exponent 
    }
    result |= (result_mant - MANTISSE_HIDDEN_BIT_MASK); // Remove hidden bit and add to result
    return result;
}

//! Knowing the high and low numbers, subtract the low from the high and return the result
static inline flpt_t flpt_sub_raw(const flpt_t high, const flpt_t low) {
    uint32_t shift = ((high & EXPONENT_MASK) - (low & EXPONENT_MASK)) >> FLPT_MANTISSE;
        if (shift > FLPT_MANTISSE) {
        return high; // low number is too small to affect the result
    }

    flpt_t result = (high & SIGN_MASK) | (high & EXPONENT_MASK);
    // Shift mantisse of low number & and high mantisse with hidden bit
    uint32_t result_mant = ((low & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> shift;
    uint32_t high_mant = (high & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK;
    result_mant = (high_mant - result_mant); // Remove hidden bit
    result -= (FLPT_MANTISSE + 1 - fl1(result_mant)) << (FLPT_MANTISSE) ; // Adjust exponent // Ignores underflow
    result_mant <<= (FLPT_MANTISSE  + 1- fl1(result_mant)); // Shift mantisse down to remove leading zeros
    result |= (result_mant - MANTISSE_HIDDEN_BIT_MASK); // Remove hidden bit and add to result
    return result;
}

static inline flpt_t flpt_mul(const flpt_t a, const flpt_t b) {
    int32_t exp_a = (a & EXPONENT_MASK);
    int32_t exp_b = (b & EXPONENT_MASK);
    int32_t exp_result = (exp_a) + (exp_b) - BIAS; // overflow shouldn't be possible
    if (((exp_a - (int32_t)MANTISSE_HIDDEN_BIT_MASK) | (exp_b - (int32_t)MANTISSE_HIDDEN_BIT_MASK) |
        (exp_result - (int32_t)MANTISSE_HIDDEN_BIT_MASK)) < 0) {
    
        return 0;
    }

    flpt_t result = exp_result;
    

    uint32_t a_mant = (((a & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK)) << FLPT_EXPONENT; // 32 bits mantisse with hidden bit
    uint32_t b_mant = (((b & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK)) << FLPT_EXPONENT;
    uint32_t result_mant = (flpt_64_mant_mul(a_mant, b_mant) >> (FLPT_EXPONENT -1));

    if (result_mant & MANTISSE_OVERFLOW_MASK) {
        result_mant >>= 1; // shift mantisse down
        result += (MANTISSE_HIDDEN_BIT_MASK); // add 1 to the exponent 
    }
    result |= (result_mant & NOT_MANTISSE_HIDDEN_BIT_MASK); // Remove hidden bit and add to result
    result |= (a ^ b) & (SIGN_MASK);
    return result;
}

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
static inline flpt_t flpt_64_mant_mul(uint32_t a_mant, uint32_t b_mant) {
    uint32_t a_high = a_mant >> 16;
    uint32_t b_high = b_mant >> 16;
    uint32_t a_low = (a_mant & MANTISSE_LOW_MASK);
    uint32_t b_low = (b_mant & MANTISSE_LOW_MASK);

    return (a_high * b_high + ((a_high * b_low) >> 16) + ((a_low * b_high) >> 16)); // ignore the low*low part
}

static inline flpt_t flpt_32_mant_mul(uint32_t *a, uint32_t *b) {
    uint32_t a_mant = ((*a & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> MANT_SHIFT; // need to implement proper way
    uint32_t b_mant = ((*b & MANTISSE_MASK) | MANTISSE_HIDDEN_BIT_MASK) >> MANT_SHIFT; // without losing precision
    return((a_mant * b_mant) >> (FLPT_EXPONENT));
}

static inline uint32_t fl1(uint32_t x) {
    uint32_t r;
    __asm__("l.fl1 %0,%1" : "=r"(r) : "r"(x));
    return r;
}

#endif // FLPT_LIB_H
