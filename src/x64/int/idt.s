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

[extern keyboardHandler]
GLOBAL ISR1
ISR1:
	PUSHALL
	call keyboardHandler
	POPALL
	iretq

[extern floppyHandler]
GLOBAL ISR6
ISR6:
	PUSHALL
	call floppyHandler
	POPALL
	iretq

[extern PITHandler]

GLOBAl ISR0
ISR0:
	PUSHALL
	call PITHandler
	POPALL
	iretq

GLOBAL exc0
GLOBAL exc1
GLOBAL exc2
GLOBAL exc3
GLOBAL exc4
GLOBAL exc5
GLOBAL exc6
GLOBAL exc7
GLOBAL exc8
GLOBAL exc9
GLOBAL exc10
GLOBAL exc11
GLOBAL exc12
GLOBAL exc13
GLOBAL exc14
GLOBAL exc16
GLOBAL exc17
GLOBAL exc18
GLOBAL exc19
GLOBAL exc20
GLOBAL exc21
GLOBAL exc28
GLOBAL exc29
GLOBAL exc30

exc0:
	PUSHALL
	mov r15, 0
	xchg bx, bx
	POPALL
	iretq
exc1:
	PUSHALL
	mov r15, 1
	xchg bx, bx
	POPALL
	iretq
exc2:
	PUSHALL
	mov r15, 2
	xchg bx, bx
	POPALL
	iretq
exc3:
	PUSHALL
	mov r15, 3
	xchg bx, bx
	POPALL
	iretq
exc4:
	PUSHALL
	mov r15, 4
	xchg bx, bx
	POPALL
	iretq
exc5:
	PUSHALL
	mov r15, 5
	xchg bx, bx
	POPALL
	iretq
exc6:
	PUSHALL
	mov r15, 6
	xchg bx, bx
	POPALL
	iretq
exc7:
	PUSHALL
	mov r15, 7
	xchg bx, bx
	POPALL
	iretq
exc8:
	PUSHALL
	mov r15, 8
	xchg bx, bx
	POPALL
	iretq
exc9:
	PUSHALL
	mov r15, 9
	xchg bx, bx
	POPALL
	iretq
exc10:
	PUSHALL
	mov r15, 10
	xchg bx, bx
	POPALL
exc11:
	PUSHALL
	mov r15, 11
	xchg bx, bx
	POPALL
exc12:
	PUSHALL
	mov r15, 12
	xchg bx, bx
	POPALL
exc13:
	PUSHALL
	mov r15, 13
	xchg bx, bx
	POPALL
exc14:
	PUSHALL
	mov r15, 14
	xchg bx, bx
	POPALL
exc16:
	PUSHALL
	mov r15, 16
	xchg bx, bx
	POPALL
exc17:
	PUSHALL
	mov r15, 17
	xchg bx, bx
	POPALL
exc18:
	PUSHALL
	mov r15, 18
	xchg bx, bx
	POPALL
exc19:
	PUSHALL
	mov r15, 19
	xchg bx, bx
	POPALL
exc20:
	PUSHALL
	mov r15, 20
	xchg bx, bx
	POPALL
exc21:
	PUSHALL
	mov r15, 21
	xchg bx, bx
	POPALL
exc28:
	PUSHALL
	mov r15, 28
	xchg bx, bx
	POPALL
exc29:
	PUSHALL
	mov r15, 29
	xchg bx, bx
	POPALL
exc30:
	PUSHALL
	mov r15, 30
	xchg bx, bx
	POPALL