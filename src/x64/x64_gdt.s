PRESENT         equ 1 << 7
NOT_SYS         equ 1 << 4
EXEC            equ 1 << 3
DC              equ 1 << 2
RW              equ 1 << 1
ACCESSED        equ 1 << 0

GRAN_4K         equ 1 << 7
SZ_32           equ 1 << 6
LONG_MODE       equ 1 << 5

x64_gdt_openOS:
    .NullSeg: equ $ - x64_gdt_openOS
        dq 0
    .CodeSeg: equ $ - x64_gdt_openOS
        dd 0xFFFF
        db 0
        db PRESENT | NOT_SYS | EXEC | RW
        db GRAN_4K | LONG_MODE | 0xF
        db 0
    .DataSeg: equ $ - x64_gdt_openOS
        dd 0xFFFF
        db 0
        db PRESENT | NOT_SYS | RW
        db GRAN_4K | SZ_32 | 0xF
        db 0
    .TSS:    equ $ - x64_gdt_openOS
        dd 0x00000068
        dd 0x00CF8900
    .Descriptor:
        dw $ - x64_gdt_openOS - 1
        dq x64_gdt_openOS

