/*
 * linker.ld - memory map and section placement for STM32L476RG
 * Flash: 1 MB at 0x08000000. SRAM1: 96 KB at 0x20000000 (RM0351 memory map)
 * Defines the symbol used in startup: _estack, _sidata, _sdata, _edata, _sbss, _ebss.
 */

/*  Records the entry symbol and keeps Reset_Handler from being discarded as unreferenced */
ENTRY (Reset_Handler)
/* The end of RAM, 0x20000000 + 96 KB. Initial stack pointer value */
_estack = 0x20018000;

/* origins and lengths from RM0351's memory map. L476 also has 32 KB of SRAM2 at 0x10000000, unused here. */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1M
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}
SECTIONS
{
    /* Must be the first section in flash because the core reads initial SP and PC from the start of flash at reset */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
    } > FLASH

    .rodata :
    {
        *(.rodata*)
    } > FLASH

    /* Initialized globals. Run addresses in RAM, initial bytes stored in flash. */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    _sidata = LOADADDR(.data);

    /* Zero initialized globals. No flash storage, zeros written at boot by Reset_Handler between _sbss and _ebss. */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM
}