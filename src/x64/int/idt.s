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