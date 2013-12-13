#ifndef _LIBKERNEL_IO_H
#define _LIBKERNEL_IO_H

#include <kernel/inttypes.h>

#define SLOW_IO_BY_JUMPING

#ifdef SLOW_IO_BY_JUMPING
#define __SLOW_DOWN_IO __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:")
#else
#define __SLOW_DOWN_IO __asm__ __volatile__("outb %al,$0x80")
#endif

#define SLOW_DOWN_IO __SLOW_DOWN_IO


#ifdef __cplusplus
extern "C" {
#endif


  int smk_request_io( int port );
  int smk_release_io( int port );
  int smk_request_iopriv();
  int smk_release_iopriv();



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
 uint16_t inw_p(uint16_t port);
  uint8_t inb_p(uint16_t port);

#ifdef __cplusplus
}       
#endif
#endif


