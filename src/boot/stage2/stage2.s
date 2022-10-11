[org 0x8000]
[bits 16]

begin:
    in al, 0x92
    or al, 2
    out 0x92, al

    cli

    lgdt [gdt_descriptor]
    
    

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp codeseg:enter32

%include "src/boot/stage2/basic_gdt.s"

[bits 32]

enter32:
    mov ax, dataseg
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax



end:
    hlt
    jmp end