OUTPUT_FORMAT(binary)

ENTRY(_start)

SEARCH_DIR(obj/kernel)

INPUT(
    kernel.elf
    stdout.elf
    mem.elf
    iobuf.elf
    disk.elf
    idt.elf
)

OUTPUT(
    bin/kernel.bin
)

SECTIONS{
    . = 0x20000;
    
    .text : ALIGN(0x1000)
    {
        *(.text)
    }
    .data : ALIGN(0x1000)
    {
        *(.data)
    }
    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }
    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
}