OUTPUT_FORMAT(binary)

ENTRY(_start)

SEARCH_DIR(obj/bootloader)

INPUT(
    stage2.elf
    bootloader.elf
    io.elf
    pic.elf
    idt.elf
    mem.elf
    fat.elf
    fdc.elf
)

OUTPUT(
    bin/boot.bin
)

SECTIONS{
    . = 0x8000;

    .text : ALIGN(0x1000)
    {
        *(.text)
    }
    .idt BLOCK(0x1000) : ALIGN(0x1000)
    {
        _idt = . ;
        . = . + 0x1000;
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