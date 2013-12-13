#include "io.h"


void outb(uint16 port,  uint8 byte)
{
 __asm__ __volatile__ ( "\n"
                         "mov %0, %%dx\n"
						 "mov %1, %%al\n"
						 "outb %%al, %%dx\n"
                   :  : "g" (port), "g" (byte)
			       : "dx","ax"
		       );
}


void outw(uint16 port, uint16 word)
{
 __asm__ __volatile__ ( "\n"
                         "mov %0, %%dx\n"
						 "mov %1, %%ax\n"
						 "outw %%ax, %%dx\n"
                   :  : "g" (port), "g" (word)
			       : "dx","ax"
		       );
}

void outl(uint16 port, uint32 l)
{
 __asm__ __volatile__ ( "\n"
                         "mov %0, %%dx\n"
						 "mov %1, %%eax\n"
						 "outl %%eax, %%dx\n"
                   :  : "g" (port), "g" (l)
			       : "dx","eax"
		       );
}


uint32 inl(uint16 port)
{
 uint32 ans = 0;
 __asm__ __volatile__ ( "\n"
                         "mov %1, %%dx\n"
			 "inl %%dx, %%eax\n"
			 "mov %%eax, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","eax"
		      );
 return ans;
}



uint16 inw(uint16 port)
{
 uint16 ans = 0;
 __asm__ __volatile__ ( "\n"
                         "mov %1, %%dx\n"
			 "inw %%dx, %%ax\n"
			 "mov %%ax, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","ax"
		      );
 return ans;
}

uint8 inb(uint16 port)
{
 char ans = 0;
 __asm__ __volatile__ ( "\n"
                         "mov %1, %%dx\n"
			 "inb %%dx, %%al\n"
			 "mov %%al, %0\n"
			
                        
                	       : "=g" (ans)  : "g" (port)
			       : "dx","ax"
		       );
 return ans;
}




void outb_p(uint16 port,  uint8 byte)
{
 outb(port, byte);
 SLOW_DOWN_IO;
}


void outw_p(uint16 port, uint16 word)
{
  outw(port,word);
  SLOW_DOWN_IO;
}

void outl_p(uint16 port, uint32 word)
{
  outl(port,word);
  SLOW_DOWN_IO;
}

uint32 inl_p(uint16 port)
{
 uint32 ans = 0;
 ans = inl(port);
 SLOW_DOWN_IO;
 return ans;
}



uint16 inw_p(uint16 port)
{
 uint16 ans = 0;
 ans = inw(port);
 SLOW_DOWN_IO;
 return ans;
}

uint8 inb_p(uint16 port)
{
 char ans = 0;
 ans = inb(port);
 SLOW_DOWN_IO;
 return ans;
}


