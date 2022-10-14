[bits 16]

section .text
global _start
_start:
    in al, 0x92
    or al, 2
    out 0x92, al

    cli

    lgdt [gdt_descriptor]
    
    

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp codeseg:enter32

section .data
%include "src/boot/stage2/i386_gdt.s"

[bits 32]

section .data
noCPUIDString:
    db 'No CPUID detected ',0xA,0xD,0
noLongModeString:
    db 'Long mode not detected',0xA,0xD,0
longModeString:
    db 'Long mode detected',0xA,0xD,0
pagingEnabledString:
    db 'Paging enabled sucessfully',0xA,0xD,0
endString:
    db 'System Halt!',0xA,0xD,0
; prints a message (null terminated) to COM1 , BX: string ptr

section .text
debugPrint:
    .loop:
    cmp [ebx], byte 0
    je .exit
        mov ax, [bx]
        mov dx, 0xe9
        out dx, ax
        inc bx
        jmp .loop
    .exit:
    ret


noCPUID:
    mov bx, noCPUIDString
    call debugPrint
    jmp end

noLongMode:
    mov bx, noLongModeString
    call debugPrint
    jmp end

enter32:

    ;Set up segment registers
    mov ax, dataseg
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Check if CPUID is supported by attempting to flip the ID bit (bit 21) in
    ; the FLAGS register. If we can flip it, CPUID is available.
 
    pushfd
    pop eax
 
    mov ecx, eax
 
    xor eax, 1 << 21
 
    push eax
    popfd
 
    pushfd
    pop eax
 
    push ecx
    popfd
 
    xor eax, ecx
    jz noCPUID
    
    ;Detect CPUID extensions

    mov eax, 0x80000000
    CPUID
    cmp eax, 0x80000001
    jb noLongMode

    ;Detect Long Mode

    mov eax, 0x80000001
    CPUID
    test edx, 1 << 29
    jz noLongMode

    mov bx, longModeString
    call debugPrint

    ;Identity-map the first 2 megabytes (0x00000000 - 0x00200000)
    ;Adresses 0x1000-0x5000 are used in order to map the page tables

    ;Clearing the memory where the tables will go

    mov edi, PM4LT                          
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd
    mov edi, cr3

    mov dword [edi], (PDPT + PAGE_ENABLE)   ;Set the value of PM4LT[0] = PDPT       (EDI currently points to the PM4LT, we are dereferencing this)

    mov edi, PDPT
    mov dword [edi], (PDT + PAGE_ENABLE)    ;Set the value of PDPT[0] = PDT

    mov edi, PDT
    mov dword [edi], (PT + PAGE_ENABLE)     ;Set the value of PDT[0] = PT

    mov bx, pagingEnabledString
    call debugPrint
    
    mov edi, PT

    ;Enabling all pages that make up the 2MB (4K per page * 512) 

    mov ebx, 0x00000003       
    mov ecx, 512

    .setPageTableEntry:
        mov dword [edi], ebx        ;Set the destination page as active
        add ebx, 0x1000             ;Add 4k so it points to the next page of memory waiting to be mapped
        add edi, 8                  ;Add 8 to the PT so we can derefernce the next page
        loop .setPageTableEntry     ;Loop 512 times (Register ECX)

    ;Enable PAE (Physical Address Extension)

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ;Enter Long Mode (amd64)

    mov ecx, 0xC0000080             ;Set to read the EFER MSR register
    rdmsr                           ;Reads the Extended Feature Enable Register(EFER) in EAX
    or eax, 1 << 8                  ;Sets the LME bit (Long Mode Enable)
    wrmsr                           ;Write the values in EAX to the EFER

    ;Enable x64 paging by setting bit 31 in CR0

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ;Enable x64 GDT and jump to 64 bit code

    lgdt [x64_gdt_openOS.Descriptor]
    jmp x64_gdt_openOS.CodeSeg:entry64


end:
    mov bx, endString
    call debugPrint
    jmp break
break:
    xchg bx, bx 
    hlt


PM4LT   equ 0x1000      ;Page Map Level 4 Table (PM4LT[0] = PDPT)           uint32_t**** (Too many pointers!!!)
PDPT    equ 0x2000      ;Page Directory Pointer Table (PDPT[0] = PDT)       uint32_t***
PDT     equ 0x3000      ;Page Directory Table (PDT[0] = PT)                 uint32_t**
PT      equ 0x4000      ;Page Table                                         uint32_t*

PAGE_ENABLE equ 0x0003  ;Page can be read and written to, page is active

section .data

%include "src/x64/x64_gdt.s"

section .text
%include "src/x64/entry.s"