OUTPUT_FORMAT(binary)

ENTRY(_start)

SEARCH_DIR(obj/kernel)

INPUT(
    kernel.elf
    vga.elf
)

OUTPUT(
    bin/kernel.bin
)

SECTIONS{
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