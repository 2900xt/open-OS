[bits 64]
entry64:

    mov ax, x64_gdt_openOS.DataSeg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax , cr0
    and ax, 0b11111101
    or ax, 0b00000001
    mov cr0, rax

    mov rax, cr4
    or ax, 0b1100000000
    mov cr4, rax


    [extern setup64]
    call setup64    

    .end:
        hlt
        jmp .end


%include "src/x64/int/idt.s"