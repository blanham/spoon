#ifndef _KERNEL_i686_IO_H
#define _KERNEL_i686_IO_H

#include <types.h>

#define SLOW_DOWN_IO __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:jmp 1f\n1:jmp 1f\n1:jmp 1f\n1:")



// Normal IN and OUT routines without IO slowdown

   void outb(uint16 port,  uint8 byte);
   void outw(uint16 port, uint16 word);
   void outl(uint16 port, uint32 word);
 uint32 inl(uint16 port);
 uint16 inw(uint16 port);
  uint8 inb(uint16 port);

// IN and OUT routines with a slight pause afterwards

   void outb_p(uint16 port,  uint8 byte);
   void outw_p(uint16 port, uint16 word);
   void outl_p(uint16 port, uint32 word);
 uint32 inw_l(uint16 port);
 uint16 inw_p(uint16 port);
  uint8 inb_p(uint16 port);

#endif

