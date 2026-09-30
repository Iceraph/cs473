#ifndef FRACTAL_FXPT_H
#define FRACTAL_FXPT_H

#include <stdint.h>

typedef int32_t q6_26;   //!< 6.26 fixed point type
typedef int64_t q12_52;  //!< 12.52 fixed point type 

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(q6_26 cx, q6_26 cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(q6_26 cx, q6_26 cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  q6_26 cx_0, q6_26 cy_0, q6_26 delta, uint16_t n_max);

#endif // FRACTAL_FXPT_H
