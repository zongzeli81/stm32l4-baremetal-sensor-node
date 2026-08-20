CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m4 -mthumb -O2 -Wall -Wextra -g -ffreestanding -fno-builtin
LDFLAGS = -T linker.ld -nostdlib -Wl,-Map=blink.map

blink.bin: blink.elf
	arm-none-eabi-objcopy -O binary $< $@

blink.elf: startup.c main.c linker.ld
	$(CC) $(CFLAGS) startup.c main.c $(LDFLAGS) -o $@
	arm-none-eabi-size $@

flash: blink.bin
	cp blink.bin /d/NODE_L476RG/

clean:
	rm -f *.elf *.bin *.map