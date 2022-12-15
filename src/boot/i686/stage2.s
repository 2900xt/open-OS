[bits 16]

section .text
global _start
_start:

    xor ax, ax
    mov es, ax
    mov di, 0x1000      ;Storing memory map at 0x1000, needs to be moved over because it interferes with DMA transfers
    mov edx, 0x534d4150
    xor ebx, ebx

    .loop:
        mov eax, 0xE820
        mov ecx, 24
        int 0x15

        cmp ebx, 0
        je .done

        add di, 24
        inc byte [memoryRegionCount]
        jmp .loop

    .done:

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
gdt_nulldesc:
	dd 0
	dd 0	
gdt_codedesc:
	dw 0xFFFF			; Limit
	dw 0x0000			; Base (low)
	db 0x00				; Base (medium)
	db 10011010b		; Flags
	db 11001111b		; Flags + Upper Limit
	db 0x00				; Base (high)
gdt_datadesc:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00

gdt_end:

gdt_descriptor:
	gdt_size: 
		dw gdt_end - gdt_nulldesc - 1
		dq gdt_nulldesc

codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

global memoryRegionCount
memoryRegionCount:
    db 0

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

    mov ebx, PAGE_ENABLE      
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


PM4LT   equ 0x4000      ;Page Map Level 4 Table (PM4LT[0] = PDPT)           uint32_t**** (Too many pointers!!!)
PDPT    equ 0x5000      ;Page Directory Pointer Table (PDPT[0] = PDT)       uint32_t***
PDT     equ 0x6000      ;Page Directory Table (PDT[0] = PT)                 uint32_t**
PT      equ 0x7000      ;Page Table                                         uint32_t*

PAGE_ENABLE equ 0x00000003  ;Page can be read and written to, page is active

section .data

PRESENT         equ 1 << 7
NOT_SYS         equ 1 << 4
EXEC            equ 1 << 3
DC              equ 1 << 2
RW              equ 1 << 1
ACCESSED        equ 1 << 0

GRAN_4K         equ 1 << 7
SZ_32           equ 1 << 6
LONG_MODE       equ 1 << 5

global x64_gdt_openOS
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



section .text

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

%macro PUSHALL 0
push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%endmacro

%macro POPALL 0
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
%endmacro

GLOBAL common_ISR
common_ISR:
	xchg bx, bx
	iretq

[extern floppyHandler]
GLOBAL ISR6
ISR6:
	PUSHALL
	call floppyHandler
	POPALL
	iretq
