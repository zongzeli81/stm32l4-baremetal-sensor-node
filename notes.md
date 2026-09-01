# Register notes - stm32l4 sensor node

## Stage 2: EXTI button (PC13 -> EXTI line 13, IRQ 40)

RCC_AHB2ENR    = 0x40021000 + 0x4C = 0x4002104C, bit 2 = GPIOCEN
RCC_APB2ENR    = 0x40021000 + 0x60 = 0x40021060, bit 0 = SYSCFGEN
    Note : "When the peripheral clock is not active, the peripheral
    registers read or write access is not supported."
SYSCFG_EXTICR4 = 0x40010000 + 0x14 = 0x40010014,
    bits 7:4 = EXTI13[3:0], value 0b0010 selects port C
EXTI_IMR1      = EXTI base 0x40010400 + 0x00 = 0x40010400, bit 13 unmasks line 13
EXTI_FTSR1     = EXTI base + 0x0C = 0x4001040C bit 13 = falling edge on line 13
EXTI_PR1       = EXTI base + 0x14 = 0x40010414 bit 13 = pending flag for line 13.
    Clearing mechanism: "cleared by writing 1 to the bit"

NVIC_ISER1 = 0xE000E104, bit 8 enables IRQ 40, EXTI15_10, write 1 to set

PC13 reset mode: analog GPIOC MODER reset value = 0xFFFFFFFF, PC13 at power-on = 0b11,

## Lessons

- Clear a pending flag first in the handler and read it back. The clear is a
  buffered store; returning before it reaches the peripheral lets the NVIC
  re-enter the handler.
- XOR observations are parity-blind. A handler entering twice per event looks
  like never entering. Count events with a set or a counter when debugging.
- EXTI_SWIER1 = 0x40010400 + 0x10 = 0x40010410, bit 13. Writing 1 fires line 13
  from software, testing EXTI to NVIC to handler without the pin.

## Stage 3: TIM2 update interrupt (1 Hz blink, IRQ 28)

TIM2 base     = 0x40000000, APB1 (RM0351 memory map)
RCC_APB1ENR1  = 0x40021000 + 0x58 = 0x40021058, bit 0 = TIM2EN (RM0351 6.4.19)
TIM2_CR1      = 0x40000000 + 0x00 = 0x40000000, bit 0 = CEN, counter enable (RM0351 31.4.1)
TIM2_DIER     = 0x40000000 + 0x0C = 0x4000000C, bit 0 = UIE, update interrupt enable (RM0351 31.4.4)
TIM2_SR       = 0x40000000 + 0x10 = 0x40000010, bit 0 = UIF, update flag (RM0351 31.4.5)
    Clearing mechanism, ST's words: "by writing 0"
TIM2_PSC      = 0x40000000 + 0x28 = 0x40000028, 16-bit prescaler (RM0351 31.4.14)
TIM2_ARR      = 0x40000000 + 0x2C = 0x4000002C, auto-reload value (RM0351 31.4.15)

TIM2 IRQ number = 28 (RM0351 vector table), NVIC_ISER0 = 0xE000E100, bit 28 (PM0214 4.3.2)

Rate math: 4 MHz / (PSC+1) / (ARR+1) = 1 Hz
    PSC = 3999, ARR = 999