#include <stdint.h>

extern uint32_t _sidata[], _sdata[], _edata[], _sbss[], _ebss[], _estack[];

void Default_Handler(void) {
    while(1);
}

void Reset_Handler(void);

int main(void);

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
    uint32_t *src = _sidata;
    uint32_t *dst = _sdata;
    while (dst < _edata) {
        *dst++ = *src++;
    }
    uint32_t *bss = _sbss;
    while (bss < _ebss) {
        *bss++ = 0;
    }
    main();
    while (1);
}