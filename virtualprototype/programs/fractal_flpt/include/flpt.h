#ifndef FLPT_H
#define FLPT_H

#include <stdint.h>

#define FLPT_EXPONENT 6
#define FLPT_SIGN 1
#define FLPT_MANTISSE (32 - FLPT_EXPONENT - FLPT_SIGN)

typedef uint32_t flpt_t;

static inline flpt_t flpt_add(const flpt_t a, const flpt_t b);

static inline flpt_t flpt_sub(const flpt_t a, const flpt_t b);

static inline flpt_t flpt_mul(const flpt_t a, const flpt_t b);

static inline flpt_t flpt_double(const flpt_t a);

static inline flpt_t flpt_square(const flpt_t a);

static inline flpt_t flpt_add_raw(const flpt_t high, const flpt_t low);
static inline flpt_t flpt_sub_raw(const flpt_t high, const flpt_t low);
static inline uint32_t fl1(uint32_t x);


#endif // FLPT_H