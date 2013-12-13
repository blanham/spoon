
# This is the SMP bootup code for AP's. It will:
# 
#    1. load the GDT
#    2. enable protected mode
#    3. flush the segment descriptors
#	 4. load the valid stack
#    5. jump into the real C kernel code.. following the BSP
#


.globl __ap_boot

.align 4096
.code16


__ap_boot:
	cli
	
	mov %cs, %ax			# ensure segments
	mov %ax, %ds


							# load the kernel GDT
	lgdtl  __gdt_Ptr -  __ap_boot + 2

							# enable protected mode
	mov %cr0, %eax
	or $1, %eax
	mov %eax, %cr0

							# flush selectors
	ljmpl $0x8, $1f

							# 32 bit protected mode
.code32
							# We should be way up past the 1 MB mark now

							# Ensure selectors
	1:
	 mov $0x10, %eax
	 mov %ax, %ds
	 mov %ax, %es
	 mov %ax, %fs
	 mov %ax, %gs
	 mov %ax, %ss

							# Sequentially use the AP stack (lock it)
 __ap_lock:
	 lock btsl $0x0, __ap_stack_lock
	 jc __ap_lock

	 mov $ap_stack, %esp 
	 add $0x1000, %esp

							# Request a page of memory for a stack
	 pushal
	 pushl $0x1
	 call memory_alloc

							# Use the new stack.
	 movl %eax, %esp
	 add $0x1000, %esp

							# Unlock the ap stack
	 movl $0x0, __ap_stack_lock	
							

							# And join the BSP processor in boot up ... 
	 pushl %eax
	 call cpu_init
	

__gdt_Ptr:
	.long 0xFFFF0000
	.long tempGDT
	
__ap_stack_lock:
	.long	0
	





