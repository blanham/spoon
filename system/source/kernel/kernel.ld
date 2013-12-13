OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(__bsp_entry);



SECTIONS
{
	. = 0x100000 ;
	_KERNEL_START = .;

	.rodata : { *(.mboot) }
	.text :	{  *(.text) }

	.data : { 
			  . = ALIGN(0x1000);
			   __data_start = .;
			    *(.data);
			   __data_end = .;
			}

  	.bss : { 
			  . = ALIGN(0x1000);
			  __bss_start = .;
				*(.bss) 
			  __bss_end = .;
		   }

		.dynamic : { *(.dynamic) }
        .comment : { *(.comment) }
           .note : { *(.note) }
       .eh_frame : { *(.eh_frame) }
          .dtors : { *(.dtors) }

	. = ALIGN(0x1000);
	_KERNEL_END = . ;
}

