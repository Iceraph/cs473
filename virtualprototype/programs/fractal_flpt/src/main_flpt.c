#include "fractal_flpt.h"
#include "swap.h"
#include "vga.h"
#include "cache.h"
#include <stddef.h>
#include <stdio.h>
#include "flpt.h"
#include "perf.h"
#include "spr.h"
void bus_error_handler() {
    printf("bus error: pc=%08x addr=%08x\n", SPR_READ(0x20), SPR_READ(0x30));
}

static rgb565 frameBuffer[512 * 512] __attribute__((aligned(32)));   // global, outside main

// Constants describing the output device
const int SCREEN_WIDTH = 512;   //!< screen width
const int SCREEN_HEIGHT = 512;  //!< screen height

// Constants describing the initial view port on the fractal function
const float FRAC_WIDTH = 3.0;  //!< default fractal width (3.0 in Q4.28)
const flpt_t CX_0 = 0xB6000000;      //!< default start x-coordinate (-2.0 in Q4.28)
const flpt_t CY_0 = 0xB5000000;      //!< default start y-coordinate (-1.5 in Q4.28)
const uint16_t N_MAX = 64;    //!< maximum number of iterations

int main() {
   volatile unsigned int *vga = (unsigned int *) 0x50000020;
   volatile unsigned int reg, hi;
   //rgb565 frameBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
   //float delta = FRAC_WIDTH / SCREEN_WIDTH;
   flpt_t delta = 0x25000000;
   printf("delta = %x\n", delta);
   int i;
   vga_clear();
   perf_init();
   printf("Starting drawing a fractal\n");
   // Perf settings
   perf_set_mask(PERF_COUNTER_0, PERF_EXECUTED_INSTRUCTIONS_MASK);
   perf_set_mask(PERF_COUNTER_1, PERF_STALL_CYCLES_MASK);
   perf_set_mask(PERF_COUNTER_2, PERF_BRANCH_PENALTY_MASK);
   perf_set_mask(PERF_COUNTER_3, PERF_DCACHE_MISS_MASK);
   perf_start();
#ifdef __OR1300__   
   /* enable the caches */
   icache_write_cfg( CACHE_DIRECT_MAPPED | CACHE_SIZE_8K | CACHE_REPLACE_FIFO );
   dcache_write_cfg( CACHE_FOUR_WAY | CACHE_SIZE_8K | CACHE_REPLACE_LRU | CACHE_WRITE_BACK );
   icache_enable(1);
   dcache_enable(1);
#endif
   /* Enable the vga-controller's graphic mode */
   vga[0] = swap_u32(SCREEN_WIDTH);
   vga[1] = swap_u32(SCREEN_HEIGHT);
   vga[2] = swap_u32(1);
   vga[3] = swap_u32((unsigned int)&frameBuffer[0]);
   /* Clear screen */
   for (i = 0 ; i < SCREEN_WIDTH*SCREEN_HEIGHT ; i++) frameBuffer[i]=0;
   printf("Cleared framebuffer\n");
   /* Draw the fractal */
   draw_fractal(frameBuffer,SCREEN_WIDTH,SCREEN_HEIGHT,&calc_mandelbrot_point_soft, &iter_to_colour,CX_0,CY_0,delta,N_MAX);
#ifdef __OR1300__
   dcache_flush();
#endif
   perf_stop();
   perf_print_cycles(PERF_COUNTER_RUNTIME, "Fractal"); 
   perf_print_time(PERF_COUNTER_RUNTIME, "Fractal");
   perf_print_cycles(PERF_COUNTER_0, "Instructions");
   perf_print_cycles(PERF_COUNTER_1, "Stall cycles");
   perf_print_cycles(PERF_COUNTER_2, "Branch penalty");
   perf_print_cycles(PERF_COUNTER_3, "D-cache misses");
   printf("Done\n");
}
