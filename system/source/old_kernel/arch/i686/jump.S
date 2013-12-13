


#define KERNEL_ENTRY \
    pushl %gs;	\
    pushl %fs;	\
    pushl %ds;	\
    pushl %es;	\
    pushl %edi;	\
    pushl %esi;	\
    pushl %ebp;	\
    pushl %edx;	\
    pushl %ecx;	\
    pushl %ebx;	\
    pushl %eax;	


#define KERNEL_EXIT \
    popl %eax;	\
    popl %ebx;	\
    popl %ecx;	\
    popl %edx;	\
    popl %ebp;	\
    popl %esi;	\
    popl %edi;	\
    popl %es;	\
    popl %ds;	\
    popl %fs;	\
    popl %gs;	\

 

.globl     __kernel_entry
.globl     __null_int

.globl     __hard_scheduler
.globl	   __soft_scheduler

.globl     __switch_entry
.globl     __switch_thread
.globl     __loop
.globl     __system_call



.globl   __exception_handler0
.globl   __exception_handler1
.globl   __exception_handler2
.globl   __exception_handler3
.globl   __exception_handler4
.globl   __exception_handler5
.globl   __exception_handler6
.globl   __exception_handler7
.globl   __exception_handler8
.globl   __exception_handler9
.globl   __exception_handlerA
.globl   __exception_handlerB
.globl   __exception_handlerC
.globl   __exception_handlerD
.globl   __exception_handlerE
.globl   __exception_handlerF
.globl   __exception_handler10
.globl   __exception_handler11
.globl   __exception_handler12
.globl   __exception_handler13
.globl   __exception_handler14
.globl   __exception_handler15
.globl   __exception_handler16
.globl   __exception_handler17
.globl   __exception_handler18
.globl   __exception_handler19
.globl   __exception_handler1A
.globl   __exception_handler1B
.globl   __exception_handler1C
.globl   __exception_handler1D
.globl   __exception_handler1E
.globl   __exception_handler1F


.globl   __irq_handler1
.globl   __irq_handler2
.globl   __irq_handler3
.globl   __irq_handler4
.globl   __irq_handler5
.globl   __irq_handler6
.globl   __irq_handler7
.globl   __irq_handler8
.globl   __irq_handler9
.globl   __irq_handlerA
.globl   __irq_handlerB
.globl   __irq_handlerC
.globl   __irq_handlerD
.globl   __irq_handlerE
.globl   __irq_handlerF




# -------------------------------------

# THIS IS THE ENTRY POINT INTO THE KERNEL FROM THE MULTIBOOT BOOT LOADER
# This is what GRUB calls when it first jumps to the kernel. It knows to
# call this entry point because it's been specified as the entry point
# in the kernel's ELF header when it was linked together. Have a look
# at the linker script.
#

__kernel_entry:
       pushl %ebx
       pushl %eax
       call arch_init
	

# -------------------------------------


__null_int:
     iret

__loop:
  int $0x30
  jmp __loop



# ---------- SCHEDULER ROUTINES -----------------

__switch_entry:
	pushl $0x0
	call switcher
	popl %eax
	iret


__switch_thread:
        pushl %eax
        pushl %ebx
        pushl %ecx
        pushl %edx
        pushl %ebp
        pushl %esi
        pushl %edi

        pushl %esp
		call set_switch_stack
		popl %esp

        addl $0x1C, %esp

	popl %eax
	popl %esp

	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %ebp
	popl %esi
	popl %edi
	
	popl %es
	popl %ds
	popl %fs
	popl %gs
	iret


__hard_scheduler:
	push %edx
	push %eax
	movl $0x20, %eax
	movl $0x20, %edx
	outb %al, %dx
	pop %eax
	pop %edx
	

__soft_scheduler:
    pushl %gs	
    pushl %fs	
    pushl %ds	
    pushl %es	
    pushl %edi	
    pushl %esi	
    pushl %ebp	
    pushl %edx	
    pushl %ecx	
    pushl %ebx	
    pushl %eax	

    pushl %esp
    call get_switch_stack
    popl %ebx

    mov %eax, %esp
    mov %ebx, %eax

    popl %edi
    popl %esi
    popl %ebp
    popl %edx
    popl %ecx
    popl %ebx

    add $0x4, %esp

    ret
    
 
     
     
# ---------------  EXCEPTION HANDLERS ---------------------------



__exception_handler0_reentry:
     iret
__exception_handler0:
       KERNEL_ENTRY
       pushl $0x0
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler0_reentry



__exception_handler1_reentry:
     iret
__exception_handler1:
       KERNEL_ENTRY
       pushl $0x1
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1_reentry

__exception_handler2_reentry:
     iret
__exception_handler2:
       KERNEL_ENTRY
       pushl $0x2
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler2_reentry

__exception_handler3_reentry:
     iret
__exception_handler3:
       KERNEL_ENTRY
       pushl $0x3
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler3_reentry



__exception_handler4_reentry:
     iret
__exception_handler4:
       KERNEL_ENTRY
       pushl $0x4
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler4_reentry

__exception_handler5_reentry:
     iret
__exception_handler5:
       KERNEL_ENTRY
       pushl $0x5
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler5_reentry

__exception_handler6_reentry:
     iret
__exception_handler6:
       KERNEL_ENTRY
       pushl $0x6
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler6_reentry

__exception_handler7_reentry:
     iret
__exception_handler7:
       KERNEL_ENTRY
       pushl $0x7
       call device_not_available
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler7_reentry



__exception_handler8_reentry:
     iret
__exception_handler8:
       KERNEL_ENTRY
       pushl $0x8
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler8_reentry

__exception_handler9_reentry:
     iret
__exception_handler9:
       KERNEL_ENTRY
       pushl $0x9
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler9_reentry

__exception_handlerA_reentry:
     iret
__exception_handlerA:
       KERNEL_ENTRY
       pushl $0xA
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handlerA_reentry

__exception_handlerB_reentry:
     iret
__exception_handlerB:
       KERNEL_ENTRY
       pushl $0xB
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handlerB_reentry



__exception_handlerC_reentry:
     iret
__exception_handlerC:
       KERNEL_ENTRY
       pushl $0xC
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handlerC_reentry

__exception_handlerD_reentry:
     iret
__exception_handlerD:
       KERNEL_ENTRY
       pushl $0xD
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handlerD_reentry

__exception_handlerE_reentry:
     iret
__exception_handlerE:
       KERNEL_ENTRY
       pushl $0xE
       call real_exception
       popl %eax
       KERNEL_EXIT
# remove the error code       
#       add $0x4, %esp
     jmp __exception_handlerE_reentry

__exception_handlerF_reentry:
     iret
__exception_handlerF:
       KERNEL_ENTRY
       pushl $0xF
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handlerF_reentry



__exception_handler10_reentry:
     iret
__exception_handler10:
       KERNEL_ENTRY
       pushl $0x10
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler10_reentry

__exception_handler11_reentry:
     iret
__exception_handler11:
       KERNEL_ENTRY
       pushl $0x11
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler11_reentry

__exception_handler12_reentry:
     iret
__exception_handler12:
       KERNEL_ENTRY
       pushl $0x12
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler12_reentry

__exception_handler13_reentry:
     iret
__exception_handler13:
       KERNEL_ENTRY
       pushl $0x13
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler13_reentry

__exception_handler14_reentry:
     iret
__exception_handler14:
       KERNEL_ENTRY
       pushl $0x14
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler14_reentry

__exception_handler15_reentry:
     iret
__exception_handler15:
       KERNEL_ENTRY
       pushl $0x15
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler15_reentry

__exception_handler16_reentry:
     iret
__exception_handler16:
       KERNEL_ENTRY
       pushl $0x16
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler16_reentry


__exception_handler17_reentry:
     iret
__exception_handler17:
       KERNEL_ENTRY
       pushl $0x17
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler17_reentry


__exception_handler18_reentry:
     iret
__exception_handler18:
       KERNEL_ENTRY
       pushl $0x18
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler18_reentry


__exception_handler19_reentry:
     iret
__exception_handler19:
       KERNEL_ENTRY
       pushl $0x19
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler19_reentry


__exception_handler1A_reentry:
     iret
__exception_handler1A:
       KERNEL_ENTRY
       pushl $0x1A
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1A_reentry


__exception_handler1B_reentry:
     iret
__exception_handler1B:
       KERNEL_ENTRY
       pushl $0x1B
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1B_reentry


__exception_handler1C_reentry:
     iret
__exception_handler1C:
       KERNEL_ENTRY
       pushl $0x1C
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1C_reentry


__exception_handler1D_reentry:
     iret
__exception_handler1D:
       KERNEL_ENTRY
       pushl $0x1D
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1D_reentry


__exception_handler1E_reentry:
     iret
__exception_handler1E:
       KERNEL_ENTRY
       pushl $0x1E
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1E_reentry


__exception_handler1F_reentry:
     iret
__exception_handler1F:
       KERNEL_ENTRY
       pushl $0x1F
       call real_exception
       popl %eax
       KERNEL_EXIT
     jmp __exception_handler1F_reentry






# ---------------  IRQ HANDLERS ---------------------------


__irq_handler1_reentry:
     iret
__irq_handler1:
       KERNEL_ENTRY
       pushl $0x1
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler1_reentry

__irq_handler2_reentry:
     iret
__irq_handler2:
       KERNEL_ENTRY
       pushl $0x2
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler2_reentry

__irq_handler3_reentry:
     iret
__irq_handler3:
       KERNEL_ENTRY
       pushl $0x3
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler3_reentry



__irq_handler4_reentry:
     iret
__irq_handler4:
       KERNEL_ENTRY
       pushl $0x4
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler4_reentry

__irq_handler5_reentry:
     iret
__irq_handler5:
       KERNEL_ENTRY
       pushl $0x5
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler5_reentry

__irq_handler6_reentry:
     iret
__irq_handler6:
       KERNEL_ENTRY
       pushl $0x6
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler6_reentry

__irq_handler7_reentry:
     iret
__irq_handler7:
       KERNEL_ENTRY
       pushl $0x7
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler7_reentry

__irq_handler8_reentry:
     iret
__irq_handler8:
       KERNEL_ENTRY
       pushl $0x8
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler8_reentry

__irq_handler9_reentry:
     iret
__irq_handler9:
       KERNEL_ENTRY
       pushl $0x9
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handler9_reentry

__irq_handlerA_reentry:
     iret
__irq_handlerA:
       KERNEL_ENTRY
       pushl $0xA
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerA_reentry

__irq_handlerB_reentry:
     iret
__irq_handlerB:
       KERNEL_ENTRY
       pushl $0xB
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerB_reentry

__irq_handlerC_reentry:
     iret
__irq_handlerC:
       KERNEL_ENTRY
       pushl $0xC
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerC_reentry

__irq_handlerD_reentry:
     iret
__irq_handlerD:
       KERNEL_ENTRY
       pushl $0xD
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerD_reentry

__irq_handlerE_reentry:
     iret
__irq_handlerE:
       KERNEL_ENTRY
       pushl $0xE
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerE_reentry

__irq_handlerF_reentry:
     iret
__irq_handlerF:
       KERNEL_ENTRY
       pushl $0xF
       call arch_irq_handler
       popl %eax
       KERNEL_EXIT
     jmp __irq_handlerF_reentry



     
# ----------------------------------------------------------------


__system_call:
	pushal
	pushl %ecx
	pushl %edx
	pushl %eax
        call syscall_prepare
        add   $0x0C, %esp
	popal
	sysexit




