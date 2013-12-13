


.global  __thread_intro




__thread_intro:
	pushl %ecx
	pushl %ebx
	call *%eax
	popl %ebx
	popl %ecx
	movl %eax, %ebx

	pushl $0x00
	pushl $0x00
	pushl $0x00
	pushl $0x00
	pushl %ebx
	pushl $0x510
	call _sysenter

	
__thread_death:
	jmp __thread_death

	

