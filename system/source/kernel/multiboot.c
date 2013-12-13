#include <multiboot.h>



#define MULTIBOOT_HEADER_FLAGS          (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VESA_MODE)
#define CHECKSUM                        -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


/* Types. */

multiboot_header_t mbinfo __attribute__ (( section (".mboot") )) = 
{ 
	MULTIBOOT_HEADER_MAGIC,
	MULTIBOOT_HEADER_FLAGS,
	CHECKSUM,
	0,
	0,
	0,
	0,
	0,
	0,
	800,
	600,
	32
};

