#include <stdio.h>
#include <perf.h>
#include <swap.h>
#include <cache.h>
#include <exception.h>
#include <spr.h>
#include <vga.h>

int main () {
   /* enable the caches */
   icache_write_cfg( CACHE_DIRECT_MAPPED | CACHE_SIZE_2K | CACHE_REPLACE_FIFO );
   dcache_write_cfg( CACHE_FOUR_WAY | CACHE_SIZE_2K | CACHE_REPLACE_LRU | CACHE_WRITE_BACK );
   icache_enable(1);
   dcache_enable(1);
   /* initialize the performance counters */
   perf_init();

   printf("Hello, world from %s (%s:%2d)!\n", __func__, __FILE__, __LINE__);

   printf("no swap: 0x%04x, swap_u16: 0x%04x\n", 0xDEAD, swap_u16(0xDEAD));
   printf("no swap: 0x%08x, swap_u32: 0x%08x\n", 0xDEADBEEF, swap_u32(0xDEADBEEF));

   perf_start();

    /*//2.5 System call

    /*void my_system_call_handler() {
        unsigned int epc;
        epc = SPR_READ(SPR_EPC);
        epc = epc-4;   // Where we point on the instruction

        //2.5.3
        /*uint16_t syscall_number;
        uint32_t instruction;
        instruction = *(volatile unsigned int *)epc;  //lire l'addresse de epc
        syscall_number = instruction & 0xFFFF;
        printf("syscall Number = 0x%04x\n", syscall_number);
        if(syscall_number == 0xE0){
            vga_clear();
        }*/

        //2.5.2
        /*printf("EPC = 0x%08x\n", epc);
        printf("EPC_previous = 0x%08x\n", epc - 4);

    }

    _vectors[EXCEPTION_SYSTEM_CALL] = my_system_call_handler;*/

    //2.5.6
    /*void system_call_handler() {
        unsigned int epc;
        epc = SPR_READ(SPR_EPC);

        printf("EPC = 0x%08x\n", epc);
        printf("EPC_previous = 0x%08x\n", epc - 4);
    }

    SYSCALL(0xE0);    //make a syscall*/

    //2.4 Exception vectors
    /*void my_alignment_exception_handler(){
        printf("alignment error!\n");
    }

    _vectors[EXCEPTION_ALLIGNMENT_EXCEPTION] = my_alignment_exception_handler;  //On remplace par notre instruction

    void my_bus_exception_error(){
        printf("bus error 2!\n");
    }

    _vectors[EXCEPTION_BUS_ERROR] = my_bus_exception_error;  //On remplace par notre instruction

    int *addr = (int *) 0x80000000;
    int data;
    asm volatile(" l.lwz %[d], 0(%[s])" : [d] "=r "(data) : [s]"r"(addr));*/

   while (1) {
       perf_print_time(PERF_COUNTER_RUNTIME, "runtime");
       perf_print_cycles(PERF_COUNTER_RUNTIME, "runtime");
       for (volatile int i = 0; i < 1000000; ++i) ;
   }
   perf_stop();

   return 0;
}
