/*
 * startup.c - vector table and reset handler for STM32L476RG
 * At reset, hardware loads word 0 of flash into SP and word 1 into PC
 * (PM0214 , reset behavior). No code runs before Reset_Handler
 */
#include <stdint.h>

// Symbols here are defined by linker.ld, declared as arrays so the bare name is the address
extern uint32_t _sidata[], _sdata[], _edata[], _sbss[], _ebss[], _estack[];

// Puts unexpected exceptions at a known address, slots get real handlers as later stages need them.
void Default_Handler(void) {
    while(1);
}

void Reset_Handler(void);

int main(void);

// Read by hardware, never called by code, which is why linker.ld wraps it in KEEP.
// Word 0 is the initial stack pointer, word 1 is the reset handler, 2-15 the Cortex-M system exceptions.
__attribute__((section(".isr_vector")))
void (*const vector_table[16])(void) = {
    (void (*)(void))_estack,
    Reset_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
};

void Reset_Handler(void) {

    // Copy .data initial values from flash (_sidata) to their RAM addresses (_sdata.._edata).
    // Initialized globals are valid after this.
    uint32_t *src = _sidata;
    uint32_t *dst = _sdata;
    while (dst < _edata) {
        *dst++ = *src++;
    }
    // .bss has no stored contents. Write zeros from _sbss to _ebss.
    uint32_t *bss = _sbss;
    while (bss < _ebss) {
        *bss++ = 0;
    }
    main();
    // main never returns.
    while (1);
}