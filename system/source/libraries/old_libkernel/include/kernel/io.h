#ifndef _LIBKERNEL_IO_H
#define _LIBKERNEL_IO_H

#define SLOW_IO_BY_JUMPING

#ifdef SLOW_IO_BY_JUMPING
#define __SLOW_DOWN_IO __asm__ __volatile__("jmp 1f\n1:\tjmp 1f\n1:")
#else
#define __SLOW_DOWN_IO __asm__ __volatile__("outb %al,$0x80")
#endif

#define SLOW_DOWN_IO __SLOW_DOWN_IO


#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif


  int smk_request_io( int port );
  int smk_release_io( int port );
  int smk_request_iopriv();
  int smk_release_iopriv();



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
 uint16 inw_p(uint16 port);
  uint8 inb_p(uint16 port);

#ifdef __cplusplus
}       
#endif
#endif


