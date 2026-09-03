# Build blink for STM32L476RG: compile both .c files for Cortex-M4
# link with linker.ld, print section sizes, produce raw binary for flashing

# -fno-builtin: stops gcc replacing the .data copy loop in startup.c with a call
# to memcpy, which is not linked in a -nostdlib build.
CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m4 -mthumb -O2 -Wall -Wextra -g -ffreestanding -fno-builtin
LDFLAGS = -T linker.ld -nostartfiles --specs=nano.specs --specs=nosys.specs -Wl,-Map=blink.map

blink.bin: blink.elf
	arm-none-eabi-objcopy -O binary $< $@

blink.elf: startup.c main.c linker.ld
	$(CC) $(CFLAGS) startup.c main.c $(LDFLAGS) -o $@
	arm-none-eabi-size $@

# Copies the binary to the ST-Link mass-storage drive root, which programs the flash.
flash: blink.bin
	cp blink.bin /d/

clean:
	rm -f *.elf *.bin *.map