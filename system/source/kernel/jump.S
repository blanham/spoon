
.globl __bsp_entry
.globl __null_int
.globl __tasking
.globl __system_call
.globl __switch_thread
.globl __soft_scheduler



# -------------------------------------

# THIS IS THE ENTRY POINT INTO THE KERNEL FROM THE MULTIBOOT BOOT LOADER
# This is what GRUB calls when it first jumps to the kernel. It knows to
# call this entry point because it's been specified as the entry point
# in the kernel's ELF header when it was linked together. Have a look
# at the linker script.
#
# Just note that the entry also replaces the stack GRUB gave us with a
# stack inside the kernel. Much safer.

__bsp_entry:
		cli
		mov $boot_stack, %esp
		add $0x1000, %esp

       	pushl %ebx
       	pushl %eax
       	call init



__null_int:
		iret
	

__tasking:
		call scheduler
		iret





__system_call:
	pusha
	call syscall_prepare
	popal
	sysexit



__switch_thread:
	pusha					# save scheduler context

    pushl %esp				# pass it to set_switch_stack
	call set_switch_stack

    addl $0x28, %esp		# jump past the esp, pusha and ret info.

	popl %esp				# pop the param passed into esp...
							#  ... thereby loading thread context

	popa					# restore registers
	popl %es				# restore selectors
	popl %ds
	popl %fs
	popl %gs

	iret					# return to the thread

	

__soft_scheduler:
    pushl %gs				# save selectors	
    pushl %fs	
    pushl %ds	
    pushl %es	
	pusha					# save registers

	movl $0xFEE000B0, %eax	# ack the apic
	movl $0x0, (%eax)

    call get_switch_stack	# recover scheduler context
	movl %esp, 0x1C(%eax)	#  .. and move the thread context into what
							#     should be eax's place on the stack

    movl %eax, %esp			# restore the scheduler context
	popa					# restore registers and return
    ret
    
 



# -------------- EXCEPTIONS ----------------------



#define EXCEPTION_HANDLER( num )		\
.globl	__exception_handler##num;		\
__exception_reentry##num:				\
	iret;								\
__exception_handler##num:				\
		pusha;							\
		pushl $0x##num;					\
		call real_exception;			\
		popl %eax;						\
		popa;							\
	jmp __exception_reentry##num;	


#define DIRECT_EXCEPTION( num, func )		\
.globl  __exception_handler##num;				\
__exception_handler##num:						\
       pusha;									\
       pushl $0x##num;							\
       call func;							\
       popl %eax;								\
       popa;									\
	   iret;



EXCEPTION_HANDLER( 0 );
EXCEPTION_HANDLER( 1 );
EXCEPTION_HANDLER( 2 );
EXCEPTION_HANDLER( 3 );
EXCEPTION_HANDLER( 4 );
EXCEPTION_HANDLER( 5 );
EXCEPTION_HANDLER( 6 );
DIRECT_EXCEPTION( 7, device_not_available );
EXCEPTION_HANDLER( 8 );
EXCEPTION_HANDLER( 9 );
EXCEPTION_HANDLER( A );
EXCEPTION_HANDLER( B );
EXCEPTION_HANDLER( C );
EXCEPTION_HANDLER( D );
EXCEPTION_HANDLER( E );
EXCEPTION_HANDLER( F );
EXCEPTION_HANDLER( 10 );
EXCEPTION_HANDLER( 11 );
EXCEPTION_HANDLER( 12 );
EXCEPTION_HANDLER( 13 );
EXCEPTION_HANDLER( 14 );
EXCEPTION_HANDLER( 15 );
EXCEPTION_HANDLER( 16 );
EXCEPTION_HANDLER( 17 );
EXCEPTION_HANDLER( 18 );
EXCEPTION_HANDLER( 19 );
EXCEPTION_HANDLER( 1A );
EXCEPTION_HANDLER( 1B );
EXCEPTION_HANDLER( 1C );
EXCEPTION_HANDLER( 1D );
EXCEPTION_HANDLER( 1E );
EXCEPTION_HANDLER( 1F );




# ---------------  IRQ HANDLERS ---------------------------


#define IRQHANDLER( num ) 		\
.globl   __irq_handler##num;	\
__irq_handler##num:				\
	pusha;						\
	pushl $0x##num;				\
	call irq_handler;			\
	popl %eax;					\
	popa;						\
	iret;

	
IRQHANDLER( 1 );
IRQHANDLER( 2 );
IRQHANDLER( 3 );
IRQHANDLER( 4 );
IRQHANDLER( 5 );
IRQHANDLER( 6 );
IRQHANDLER( 7 );
IRQHANDLER( 8 );
IRQHANDLER( 9 );
IRQHANDLER( A );
IRQHANDLER( B );
IRQHANDLER( C );
IRQHANDLER( D );
IRQHANDLER( E );
IRQHANDLER( F );
 



