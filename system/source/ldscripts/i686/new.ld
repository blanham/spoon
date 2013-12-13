OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

SEARCH_DIR("/spoon/system/libraries/");

ENTRY(_start);
EXTERN( _start );
GROUP( libapp.a libdc99.a libkernel.a );
SECTIONS
{
	. = 0x80000000 ;

	.text :	{ *(.text) }

	__ctor_list = .;
	.ctors : { *(.ctors) }
	__ctor_end = .;

	.rodata : { *(.rodata) }


	. = ALIGN(0x1000);
	__data_start = .;
	.data : { *(.data*) }

	__bss_start = .;
  	.bss : { *(.bss) }

	.dynamic : { *(.dynamic) }


        .comment : { *(.comment) }
           .note : { *(.note) }
       .eh_frame : { *(.eh_frame) }
          .dtors : { *(.dtors) }

	. = ALIGN(0x1000);
	_end = . ;
}
