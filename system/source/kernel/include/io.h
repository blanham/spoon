#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <inttypes.h>

#define SLOW_DOWN_IO asm ("jmp 1f\n1:\tjmp 1f\n1:jmp 1f\n1:jmp 1f\n1:jmp 1f\n1:")


// Normal IN and OUT routines without IO slowdown

   void outb(uint16_t port,  uint8_t byte);
   void outw(uint16_t port, uint16_t word);
   void outl(uint16_t port, uint32_t word);
 uint32_t inl(uint16_t port);
 uint16_t inw(uint16_t port);
  uint8_t inb(uint16_t port);

// IN and OUT routines with a slight pause afterwards

   void outb_p(uint16_t port,  uint8_t byte);
   void outw_p(uint16_t port, uint16_t word);
   void outl_p(uint16_t port, uint32_t word);
 uint32_t inw_l(uint16_t port);
 uint16_t inw_p(uint16_t port);
  uint8_t inb_p(uint16_t port);

#endif

