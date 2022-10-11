CROSS = /usr/local/x86_64elfgcc/bin/x86_64-elf-g++
LD = /usr/local/x86_64elfgcc/bin/x86_64-elf-ld
CCFLAGS = -Ttext 0x8000 -ffreestanding -mno-red-zone -m64 -I T-DOS/include -I TFS -Wwrite-strings

all:

	rm debug/bochs.log
	touch debug/bochs.log

	nasm -o bin/bootsect.bin -fbin src/boot/bootsect.s

	nasm -o bin/EXT.elf -fbin src/boot/stage2/stage2.s


	dd if=/dev/zero of=bin/open-os.flp bs=512 count=2880
	cat bin/bootsect.bin bin/EXT.elf> bin/os.bin
	dd if=bin/os.bin of=bin/open-os.flp conv=notrunc

	bochs -q

	