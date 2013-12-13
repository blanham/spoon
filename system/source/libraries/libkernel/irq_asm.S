

.global  __irq_intro


__irq_intro:
    pusha
	pushl %ecx
	pushl %ebx
	pushl %eax
	call smk_irq_stub
	popl %eax
	popl %ebx
	popl %ecx
	popa

	pushl $0x00
	pushl $0x00
	pushl $0x00
	pushl %ebx
	pushl %eax
	pushl $0xD04
	call _sysenter
	add $0x18, %esp
	
	__irq_halted: jmp __irq_halted

	

