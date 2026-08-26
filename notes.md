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