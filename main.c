#include <stdint.h>

volatile uint32_t *RCC_AHB2ENR = (volatile uint32_t *)0x4002104C;
volatile uint32_t *GPIOA_MODER = (volatile uint32_t *)0x48000000;
volatile uint32_t *GPIOA_ODR = (volatile uint32_t *)0x48000014;

int main(void) {

*RCC_AHB2ENR |= (1u << 0);
(void)*RCC_AHB2ENR;

*GPIOA_MODER &= ~(3u <<10);
*GPIOA_MODER |= (1u << 10);

while (1) {
    *GPIOA_ODR ^= (1u << 5);
    for (volatile uint32_t i = 0; i < 400000; i++);
}

}