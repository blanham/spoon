OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

SEARCH_DIR("/spoon/system/libraries/");
ENTRY(_start);
EXTERN( _start );
GROUP( libkernel.a libstdc++.a  );
SECTIONS
{
	. = 0x80000000 ;
	.text :	{ *(.text) }

	. = ALIGN(0x1000);
	__ctor_list = .;
	.ctors : { *(.ctors) }
	__ctor_end = .;

	.rodata : { *(.rodata) }

	. = ALIGN(0x1000);
	__data_start = .;
	.data : { *(.data*) }

	. = ALIGN(0x1000);
	__bss_start = .;
  	.bss : { *(.bss)  }
	__bss_end = .;


	.dynamic : { *(.dynamic) }
        .comment : { *(.comment) }
           .note : { *(.note) }
       .eh_frame : { *(.eh_frame) }
	
	. = ALIGN(0x1000);
         __dtor_list = .;
          .dtors : { *(.dtors) }
	 __dtor_end = .;

	. = ALIGN(0x1000);
	_end = . ;
}

