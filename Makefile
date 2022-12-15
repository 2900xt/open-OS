CROSS = /usr/local/x86_64elfgcc/bin/x86_64-elf-g++
LD = /usr/local/x86_64elfgcc/bin/x86_64-elf-ld -z max-page-size=0x1000 -Ttext 0x8000
CCFLAGS = -Ttext 0x8000 -ffreestanding -mno-red-zone -m64 -Wwrite-strings -nostdlib -I lib/boot -I lib
CROSS += $(CCFLAGS)

BOOTLOADER = src/boot/amd64
KERNEL = src/kernel
OBJDIR_BOOT = obj/bootloader
OBJDIR_KERNEL = obj/kernel

.PHONY: test

test: bootloader kernel img run


bootloader:

	rm debug/bochs.log
	touch debug/bochs.log

	nasm -o bin/bootsect.bin -fbin src/boot/bootsect.s

	nasm -o $(OBJDIR_BOOT)/stage2.elf -felf64 src/boot/i686/stage2.s

	$(CROSS)  -o $(OBJDIR_BOOT)/bootloader.elf -c $(BOOTLOADER)/init.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/io.elf -c $(BOOTLOADER)/io/io.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/tty.elf -c $(BOOTLOADER)/io/tty.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/idt.elf -c $(BOOTLOADER)/int/idt.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/pic.elf -c $(BOOTLOADER)/int/pic.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/mem.elf -c $(BOOTLOADER)/mem/mem.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/fdc.elf -c $(BOOTLOADER)/fs/fdc.cpp
	$(CROSS) -o $(OBJDIR_BOOT)/fat.elf -c $(BOOTLOADER)/fs/fat.cpp

	$(LD) -T src/boot/link.ld

kernel:
	$(CROSS) -o $(OBJDIR_KERNEL)/kernel.elf -c $(KERNEL)/kernel.cpp
	$(CROSS) -o $(OBJDIR_KERNEL)/vga.elf -c $(KERNEL)/vga.cpp
	$(LD) -T $(KERNEL)/link.ld

img:

	dd if=/dev/zero of=bin/open-os.flp bs=512 count=2880
	mkfs.fat -F 12 -n "OPEN-OS" bin/open-os.flp
	dd if=bin/bootsect.bin of=bin/open-os.flp conv=notrunc
	mcopy -i bin/open-os.flp bin/boot.bin "::boot.bin"
	mcopy -i bin/open-os.flp bin/kernel.bin "::kernel.bin"
	rm debug/img.txt bin/boot.bin
	xxd -d -a -e bin/open-os.flp debug/img.txt


run:
	bochs -q