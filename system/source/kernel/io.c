#include <io.h>


void outb(uint16_t port,  uint8_t byte)
{
 asm ( "mov %0, %%dx\n"
		"mov %1, %%al\n"
		"outb %%al, %%dx\n"
	    :  : "g" (port), "g" (byte)
		: "dx","ax"
	);
}


void outw(uint16_t port, uint16_t word)
{
 asm ( "\n"
                         "mov %0, %%dx\n"
						 "mov %1, %%ax\n"
						 "outw %%ax, %%dx\n"
                   :  : "g" (port), "g" (word)
			       : "dx","ax"
		       );
}

void outl(uint16_t port, uint32_t l)
{
 asm ( "\n"
                         "mov %0, %%dx\n"
						 "mov %1, %%eax\n"
						 "outl %%eax, %%dx\n"
                   :  : "g" (port), "g" (l)
			       : "dx","eax"
		       );
}


uint32_t inl(uint16_t port)
{
 uint32_t ans = 0;
 asm ( "\n"
                         "mov %1, %%dx\n"
			 "inl %%dx, %%eax\n"
			 "mov %%eax, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","eax"
		      );
 return ans;
}



uint16_t inw(uint16_t port)
{
 uint16_t ans = 0;
 asm ( "\n"
                         "mov %1, %%dx\n"
			 "inw %%dx, %%ax\n"
			 "mov %%ax, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","ax"
		      );
 return ans;
}

uint8_t inb(uint16_t port)
{
 char ans = 0;
 asm ( "\n"
          "mov %1, %%dx\n"
			 "inb %%dx, %%al\n"
			 "mov %%al, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","ax"
		       );
 return ans;
}




void outb_p(uint16_t port,  uint8_t byte)
{
 outb(port, byte);
 SLOW_DOWN_IO;
}


void outw_p(uint16_t port, uint16_t word)
{
  outw(port,word);
  SLOW_DOWN_IO;
}

void outl_p(uint16_t port, uint32_t word)
{
  outl(port,word);
  SLOW_DOWN_IO;
}

uint32_t inl_p(uint16_t port)
{
 uint32_t ans = 0;
 ans = inl(port);
 SLOW_DOWN_IO;
 return ans;
}



uint16_t inw_p(uint16_t port)
{
 uint16_t ans = 0;
 ans = inw(port);
 SLOW_DOWN_IO;
 return ans;
}

uint8_t inb_p(uint16_t port)
{
 char ans = 0;
 ans = inb(port);
 SLOW_DOWN_IO;
 return ans;
}


