#include <inttypes.h>




void sysenter_prepare( uint32_t cs, uint32_t esp, uint32_t eip )
{
	__asm__ ("\n"
			"xor %%edx, %%edx\n"
			"mov %0, %%eax\n"
			"mov $0x174, %%ecx\n"
			"wrmsr\n"

			"xor %%edx, %%edx\n"
			"mov %1, %%eax\n"
			"mov $0x175, %%ecx\n"
			"wrmsr\n"

			"xor %%edx, %%edx\n"
			"mov %2, %%eax\n"
			"mov $0x176, %%ecx\n"
			"wrmsr\n"
		
		:
		: "g" (cs), "g" (esp), "g" (eip) 
		: "eax","edx","ecx"
	);
}


void sysenter_set_esp( uint32_t esp )
{
	__asm__ ("\n"
			"xor %%edx, %%edx\n"
			"mov %0, %%eax\n"
			"mov $0x175, %%ecx\n"
			"wrmsr\n"
		
		:
		: "g" (esp) 
		: "eax","edx","ecx"
	);

}


