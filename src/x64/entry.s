[bits 64]
entry64:

    mov ax, x64_gdt_openOS.DataSeg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    [extern kmain]
    call kmain    

    .end:
        hlt
        jmp .end


%include "src/x64/int/idt.s"