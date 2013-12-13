
.globl _start


_start:
	pushl %edx
	pushl %ecx
	pushl %edx
	pushl %eax
	call spoon_start
	popl %edx
	popl %edx
	popl %edx
	popl %edx
	movl %eax, %ebx

        pushl $0x00
        pushl $0x00
        pushl $0x00
        pushl $0x00
        pushl %ebx
        pushl $0x404
        call _sysenter

