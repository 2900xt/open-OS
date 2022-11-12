CROSS = /usr/local/x86_64elfgcc/bin/x86_64-elf-g++
LD = /usr/local/x86_64elfgcc/bin/x86_64-elf-ld -z max-page-size=0x1000 -Ttext 0x8000
CCFLAGS = -Ttext 0x8000 -ffreestanding -mno-red-zone -m64 -Wwrite-strings -nostdlib -I lib
CROSS += $(CCFLAGS)

.PHONY: test all run

test: all run


all:

	rm debug/bochs.log
	touch debug/bochs.log

	nasm -o bin/bootsect.bin -fbin src/boot/bootsect.s 

	nasm -o bin/stage2.elf -felf64 src/boot/stage2/stage2.s

	$(CROSS)  -o bin/kmain.elf -c src/x64/kmain.cpp
	$(CROSS) -o bin/io.elf -c src/x64/io/io.cpp
	$(CROSS) -o bin/tty.elf -c src/x64/io/tty.cpp
	$(CROSS) -o bin/idt.elf -c src/x64/int/idt.cpp
	$(CROSS) -o bin/pic.elf -c src/x64/int/pic.cpp
	$(CROSS) -o bin/mem.elf -c src/x64/mem/mem.cpp
	$(CROSS) -o bin/fdc.elf -c src/x64/fs/fdc.cpp
	$(CROSS) -o bin/proc.elf -c src/x64/mem/proc.cpp
	$(CROSS) -o bin/time.elf -c src/x64/io/time.cpp
	$(CROSS) -o bin/shell.elf -c src/x64/ui/shell.cpp
	$(CROSS) -o bin/fat.elf -c src/x64/fs/fat.cpp


	$(LD) -T "link.ld"

	dd if=/dev/zero of=bin/open-os.flp bs=512 count=2880
	mkfs.fat -F 12 -n "OPEN-OS" bin/open-os.flp
	dd if=bin/bootsect.bin of=bin/open-os.flp conv=notrunc
	mcopy -i bin/open-os.flp bin/Kernel.bin "::kernel.bin"
	rm debug/img.txt
	xxd -d -a -e bin/open-os.flp debug/img.txt


run:
	bochs -q





