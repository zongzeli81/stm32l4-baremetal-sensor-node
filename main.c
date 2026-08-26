/*
 * main.c - stage 1: blink LD2 (PA5) from registers, no HAL.
 * Register addresses from RM0351; board wiring from UM1724.
 */

#include <stdint.h>

/* RCC_AHB2ENR at 0x40021000 + 0x4C: peripheral clock enables
   GPIOA register block at 0x48000000: MODER at +0x00, ODR at +0x14
   PA5 drives LD2 through a resistor. */
volatile uint32_t *RCC_AHB2ENR = (volatile uint32_t *)0x4002104C;
volatile uint32_t *GPIOA_MODER = (volatile uint32_t *)0x48000000;
volatile uint32_t *GPIOA_ODR = (volatile uint32_t *)0x48000014;

int main(void) {

/* Enable GPIOA clock (bit 0). Peripherals ignore the bus until clocked.
   The discarded read gives the enable time to take effect before the first access to GPIOA. */
*RCC_AHB2ENR |= (1u << 0);
(void)*RCC_AHB2ENR;

/* MODER: two bits per pin; PA5 is bits 11:10, 01 = output. Clear then set only that field.
   Other fields keep their reset values, including the SWD debug pins PA13/PA14, which can not be changed*/
*GPIOA_MODER &= ~(3u <<10);
*GPIOA_MODER |= (1u << 10);

while (1) {
    /* XOR flips ODR bit 5: PA5 output level inverts, LED changes state. */
    *GPIOA_ODR ^= (1u << 5);
    /* Delay: ~400000 iterations at the 4 MHz reset clock gives a visible rate.
       volatile forces every iteration. */
    for (volatile uint32_t i = 0; i < 400000; i++);
}

}