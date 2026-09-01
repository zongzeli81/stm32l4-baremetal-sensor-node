/*
 * startup.c - vector table and reset handler for STM32L476RG (stage 1+2+3)
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

void EXTI15_10_IRQHandler(void);

void TIM2_IRQHandler(void);

int main(void);

// Read by hardware, never called by code, which is why linker.ld wraps it in KEEP.
// Word 0 is the initial stack pointer, word 1 is the reset handler, 2-15 the Cortex-M system exceptions.
// Peripheral IRQs start at slot 16: EXTI15_10 is IRQ 40, so its handler sits at slot 56.
// Every other slot is parked in Default_Handler until a stage claims it.
// TIM2 is IRQ 28, slot 44.
__attribute__((section(".isr_vector")))
void (*const vector_table[57])(void) = {
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
    TIM2_IRQHandler,
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
    EXTI15_10_IRQHandler,
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