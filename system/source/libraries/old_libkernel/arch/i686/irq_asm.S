

.global  __irq_intro


__irq_intro:
    pusha
	pushl %ecx
	pushl %ebx
	call *%eax
	popl %ebx
	popl %ecx

	pushl $0x00
	pushl $0x00
	pushl $0x00
	pushl %ebx
	pushl %eax
	pushl $0xD04
	call _sysenter
	add $0x18, %esp
	
	popa
	jmp __irq_intro

	

