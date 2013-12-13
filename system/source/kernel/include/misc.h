#ifndef _KERNEL_MISC_H
#define _KERNEL_MISC_H


void zero_bss();
int strlen(const char *src);
int strnlen(const char *src, int n);
char *strncpy(char *dest, const char *src, int n);
int strncmp( const char* a, const char* b, int n);

int 	memcmp(const void* s1, const void* s2, int n);
void* memcpy(void* s1, const void* s2, int n);
void* memset(void* dest, int c, int n);


void call_task( uint32_t gdt );
void jump_task( uint32_t gdt );


static inline unsigned int cr0()
{
	unsigned int cr;
	asm (" movl %%cr0, %%eax\n "
		 " movl %%eax, %0\n"
					 : "=g" (cr)  );
	return cr;
}

static inline unsigned int cr2()
{
	unsigned int cr;
	asm (" movl %%cr2, %%eax\n "
		 " movl %%eax, %0\n"
					 : "=g" (cr)  );
	return cr;
}

static inline unsigned int cr3()
{
	unsigned int cr;
	asm (" movl %%cr3, %%eax\n "
		 " movl %%eax, %0\n"
					 : "=g" (cr)  );
	return cr;
}

static inline void invlpg( uint32_t eax )
{
	__asm__
		(
		 "\n"
		   "mov %0, %%eax;\n"
		   "invlpg (%%eax);\n"
		 
		 : 
		 : "g" (eax)
		 : "eax"
		);
}

static inline uint32_t cpu_flags()
{
  uint32_t old_flags;
    asm ( 
		  "pushfl\n"
		  "pop %%eax\n"
		  "mov %%eax, %0\n"
		: "=g" (old_flags)
		:
		: "eax"
      );
      
  return old_flags;
}

static inline void set_cpu_flags( uint32_t flags )
{
    asm ( 
		  "mov %0, %%eax\n"
		  "pushl %%eax\n"
		  "popfl\n"
		: 
		: "g" (flags)
		: "eax"
      );
}



#endif

