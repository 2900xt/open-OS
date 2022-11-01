CROSS = /usr/local/x86_64elfgcc/bin/x86_64-elf-g++
LD = /usr/local/x86_64elfgcc/bin/x86_64-elf-ld -z max-page-size=0x1000 -Ttext 0x8000
CCFLAGS = -Ttext 0x8000 -ffreestanding -mno-red-zone -m64 -Wwrite-strings -nostdlib -I lib

all:

	rm debug/bochs.log
	touch debug/bochs.log

	nasm -o bin/bootsect.bin -fbin src/boot/bootsect.s 

	nasm -o bin/stage2.elf -felf64 src/boot/stage2/stage2.s

	$(CROSS) $(CCFLAGS) -o bin/kmain.elf -c src/x64/kmain.cpp
	$(CROSS) $(CCFLAGS) -o bin/io.elf -c src/x64/io/io.cpp
	$(CROSS) $(CCFLAGS) -o bin/tty.elf -c src/x64/io/tty.cpp
	$(CROSS) $(CCFLAGS) -o bin/idt.elf -c src/x64/int/idt.cpp
	$(CROSS) $(CCFLAGS) -o bin/pic.elf -c src/x64/int/pic.cpp
	$(CROSS) $(CCFLAGS) -o bin/mem.elf -c src/x64/mem/mem.cpp
	$(CROSS) $(CCFLAGS) -o bin/tfs.elf -c src/x64/fs/tfs.cpp
	$(CROSS) $(CCFLAGS) -o bin/fdc.elf -c src/x64/fs/fdc.cpp
	$(CROSS) $(CCFLAGS) -o bin/proc.elf -c src/x64/mem/proc.cpp


	$(LD) -T "link.ld"

	dd if=/dev/zero of=bin/open-os.flp bs=512 count=2880
	cat bin/bootsect.bin bin/Kernel.bin > bin/os.bin
	dd if=bin/os.bin of=bin/open-os.flp conv=notrunc

	bochs -q