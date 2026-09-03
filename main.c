/*
 *  main.c - stage 1+2+3+4: blink from TIM2, button via EXTI, printf over USART2.
 */

#include <stdint.h>
#include <stdio.h>

/* RCC_AHB2ENR at 0x40021000 + 0x4C: peripheral clock enables
   GPIOA register block at 0x48000000: MODER at +0x00, ODR at +0x14
   PA5 drives LD2 through a resistor. PC13 is user button B1
   GPIOC block at 0x48000800. SYSCFG at 0x40010000, EXTICR4 at +0x14
   EXTI at 0x40010400: IMR1 +0x00, FTSR1 +0x0C, PR1 +0x14.
   NVIC_ISER1 at 0xE000E104: IRQs 32-63. */
volatile uint32_t *RCC_AHB2ENR = (volatile uint32_t *)0x4002104C;
volatile uint32_t *GPIOA_MODER = (volatile uint32_t *)0x48000000;
volatile uint32_t *GPIOA_ODR = (volatile uint32_t *)0x48000014;
volatile uint32_t *GPIOC_MODER = (volatile uint32_t *)0x48000800;
volatile uint32_t *RCC_APB2ENR = (volatile uint32_t *)0x40021060;
volatile uint32_t *SYSCFG_EXTICR4 = (volatile uint32_t *)0x40010014;
volatile uint32_t *EXTI_IMR1 = (volatile uint32_t *)0x40010400;
volatile uint32_t *EXTI_FTSR1 = (volatile uint32_t *)0x4001040C;
volatile uint32_t *EXTI_PR1 = (volatile uint32_t *)0x40010414;
volatile uint32_t *NVIC_ISER1 = (volatile uint32_t *)0xE000E104;
/* TIM2 at 0x40000000 (APB1): CR1 +0x00, DIER +0x0C, SR +0x10, PSC +0x28, ARR +0x2C.
   NVIC_ISER0 at 0xE000E100: IRQs 0-31.*/
volatile uint32_t *RCC_APB1ENR1 = (volatile uint32_t *)0x40021058;
volatile uint32_t *TIM2_CR1 = (volatile uint32_t *)0x40000000;
volatile uint32_t *TIM2_DIER = (volatile uint32_t *)0x4000000C;
volatile uint32_t *TIM2_SR = (volatile uint32_t *)0x40000010;
volatile uint32_t *TIM2_PSC = (volatile uint32_t *)0x40000028;
volatile uint32_t *TIM2_ARR = (volatile uint32_t *)0x4000002C;
volatile uint32_t *NVIC_ISER0 = (volatile uint32_t *)0xE000E100;
volatile uint32_t *GPIOA_AFRL = (volatile uint32_t *)0x48000020;
/* USART2 at 0x40004400 (APB1): CR1 +0x00, BRR +0x0C, ISR +0x1C, TDR +0x28.
   GPIOA_AFRL at +0x20: four bits per pin select the alternate function. */
volatile uint32_t *USART2_BRR = (volatile uint32_t *)0x4000440C;
volatile uint32_t *USART2_CR1 = (volatile uint32_t *)0x40004400;
volatile uint32_t *USART2_ISR = (volatile uint32_t *)0x4000441C;
volatile uint32_t *USART2_TDR = (volatile uint32_t *)0x40004428;

/* Poll TXE (ISR bit 7): wait until TDR can take a byte, then write it. */
void uart_putc(char c) {
   while (!(*USART2_ISR & (1u << 7)));
   *USART2_TDR = c;
}

/* printf delivers its bytes here (newlib retarget). Loop them out the UART. */
int _write(int fd, const void *buf, unsigned int len) {
   (void)fd;
   const char *p = buf;
   for (unsigned int i = 0; i < len; i++) uart_putc(p[i]);
   return (int)len;
}

int main(void) {

/* Clocks first: GPIOA (bit 0) and GPIOC (bit 2) on AHB2, SYSCFG on APB2 bit 0.
   Peripherals ignore the bus until clocked. Discarded reads let the enables
   take effect before the first access. */
*RCC_AHB2ENR |= (1u << 2);
*RCC_AHB2ENR |= (1u << 0);
*RCC_APB2ENR |= (1u << 0);
*RCC_APB1ENR1 |= (1u << 0);
*RCC_APB1ENR1 |= (1u << 17);
(void)*RCC_APB1ENR1;
(void)*RCC_APB1ENR1;
(void)*RCC_AHB2ENR;
(void)*RCC_APB2ENR;


/* MODER: two bits per pin; PA5 is bits 11:10, 01 = output. Clear then set only that field.
   Other fields keep their reset values, including the SWD debug pins PA13/PA14, which can not be changed*/
*GPIOA_MODER &= ~(3u <<10);
*GPIOA_MODER |= (1u << 10);
*GPIOA_MODER &= ~(3u << 4);
*GPIOA_MODER |= (2u << 4);
*GPIOA_AFRL &= ~(0xFu << 8);
*GPIOA_AFRL |= (7u << 8);
/* GPIOC resets to analog mode (MODER = 0xFFFFFFFF): Schmitt trigger is
   disconnected and edges never reach EXTI. Clear bits 27:26 to 00 = input. */
*GPIOC_MODER &= ~(3u << 26);
/* Route port C onto EXTI line 13: EXTICR4 bits 7:4 = 0b0010. */
*SYSCFG_EXTICR4 &= ~(0xFu << 4);
*SYSCFG_EXTICR4 |= (2u << 4);
/* Unmask line 13 and select falling edge: B1 grounds PC13 when pressed. */
*EXTI_IMR1 |= (1u << 13);
*EXTI_FTSR1 |= (1u << 13);

/* TIM2 on APB1 (clocked above). PSC = 3999 divides 4 MHz to 1 kHz; ARR = 999
   gives an update every 1000 counts = 1 per second. UIE enables the update
   interrupt. CEN starts the counter, set last so PSC and ARR are in place. */
*TIM2_PSC = 3999;
*TIM2_ARR = 999;
*TIM2_DIER |= (1u << 0);
/* Enable IRQ 40 (EXTI15_10) in the NVIC: ISER1 bit 8, write-1-to-set. */
*NVIC_ISER1 |= (1u << 8);
/* Enable IRQ 28 (TIM2) in the NVIC: ISER0 bit 28. */
*NVIC_ISER0 |= (1u << 28);
*TIM2_CR1 |= (1u << 0);

/* PA2 = USART2_TX via AF7 (ds10198 AF table). BRR = 4 MHz / 115200 = 35
   (0.8% error). BRR must be written before UE sets. TE + UE enable. */
*USART2_BRR = 35;
*USART2_CR1 |= (1u << 3) | (1u << 0);

printf("uart up, BRR=%lu\r\n", (unsigned long)*USART2_BRR);

while (1);

}

/* Clear the pending flag first and read it back: the store is buffered, and
   returning before it reaches EXTI lets the NVIC re-enter this handler.
   With a toggle, the double entry cancels itself and the LED never moves. */
void EXTI15_10_IRQHandler(void) {
    *EXTI_PR1 = (1u << 13);
    (void)*EXTI_PR1;
    *GPIOA_ODR ^= (1u << 5);
}

/* UIF clears by writing 0 (rc_w0), the opposite of EXTI_PR1. The &= ~ writes
   0 to bit 0 and leaves the other flags as read. Readback drains the store. */
void TIM2_IRQHandler(void) {
   *TIM2_SR &= ~(1u << 0);
   (void)*TIM2_SR;
   *GPIOA_ODR ^= (1u << 5);
}