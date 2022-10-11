[org 0x7c00]
PROGRAM_SPACE equ 0x8000

jmp short start
nop

start:


    mov bp, 0x7c00
    mov sp, bp


    mov bx, PROGRAM_SPACE
    mov al, 64
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    call discRead

    jmp PROGRAM_SPACE


;
;DISK_READ : READS a disk using int 13h ah = 2
;
;BX = Output Location
;AL = Sectors
;CH = Ending Cylinder
;DH = Head
;CL = Starting Sector
;DL = Disk #


discRead:

    mov ah , 0x02
    int 0x13

    jc DiskReadFailed
    mov bx, DiskReadSucessString
    call PrintStr
    ret

DiskReadErrorString:
    db 'Disk Read Failed', 0
DiskReadSucessString:
    db 'Loading Open-OS..',0
DiskReadFailed:
    mov bx, DiskReadErrorString
    call PrintStr
    jmp $


;PrintString: Prints string in [BX] to TTY

PrintStr:
    mov ah, 0x0e
    .loop:
    cmp[bx], byte 0
    je .exit
        call PrintChar
        inc bx
        jmp .loop
    .exit:
    ret

PrintChar:
    mov al,[bx]
    int 0x10
    ret

times 510-($-$$) db 0;
dw 0xaa55;