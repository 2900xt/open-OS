[org 0x7c00]
[bits 16]
PROGRAM_SPACE equ 0x8000

jmp short start
nop

BPB:
    .vendorID:              db 'MSWIN4.1'               ;Vendor ID
    .BytesPerSector:        dw 512                      ;Bytes per sector 
    .ClusterSize:           db 1                        ;Cluster Size(sectors)
    .Reserved:              dw 1                        ;Reserved sectors
    .FATCount:              db 2                        ;FAT count
    .DirectoryEntryCount:   dw 0E0h                     ;Directory Entries Count
    .SectorCount:           dw 2880                     ;Total Sectors
    .MediaType:             db 0F0h                     ;Media Type (1.44m Floppy)
    .SectorsPerFat:         dw 9                        ;Sectors per FAT
    .SectorsPerTrack:       dw 18                       ;Sectors per track
    .HeadCount:             dw 2                        ;Head count
                            dd 0
                            dd 0

EBR:
    .DiskNumber:        db 0                        ;Floppy drive
                        db 0
    .Signature:         db 29h                      ;Signature
    .Serial:            db 12h, 34h, 56h, 78h       ;Serial num
    .Label:             db 'OPEN-OS    '            ;Label
    .IDString:          db 'FAT12   '               ;ID String


start:

    ;Set up stack

    mov bp, 0x7c00
    mov sp, bp

    ;Read the first file in the root directory and start executing it

    mov [EBR.DiskNumber], dl

    mov bx, PROGRAM_SPACE
    mov cl, 64
    mov ax, 33
    call discRead

    ;Call video BIOS

    mov ah, 0x00
    mov al, 0x12
    int 0x10

    ;Enter Kernel

    jmp PROGRAM_SPACE
    .end:
        jmp .end




;LBA to CHS
;ax : LBA address
;
;Returns:
;ch : cyl
;dh : head
;cl : sector

LBAtoCHS:

    push ax
    push dx

    xor dx, dx
    div word [BPB.SectorsPerTrack]
    inc dx
    mov cx, dx
    
    xor dx, dx
    div word [BPB.HeadCount]
    mov dh, dl
    mov ch, al

    shl ah, 6
    or cl, ah
    pop ax
    mov dl, al
    pop ax

    ret

;
;DISK_READ : READS a disk using int 13h ah = 2
;
;AX = LBA address
;CL = sectors to read
;BX = buffer
;DL = Disk #


discRead:
    push cx

    call LBAtoCHS

    pop ax

    mov ah , 0x02
    int 13h

    jc DiskReadFailed
    ret

DiskReadFailed:
    mov ah, 01h
    mov dl , 0
    int 13h

    mov bx, DiskReadErrorString
    call PrintStr
    jmp $

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

DiskReadErrorString:
    db 'Disk Read Failed', 0


times 510-($-$$) db 0;
dw 0xaa55;