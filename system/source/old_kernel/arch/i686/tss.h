#include <types.h>


#ifndef _KERNEL_TSS_H_
#define _KERNEL_TSS_H_


struct TSS
{
   uint16 previous, empty1;
   uint32 esp0;
   uint16 ss0, empty2;   
   uint32 esp1;
   uint16 ss1, empty3;
   uint32 esp2;
   uint16 ss2, empty4;
   uint32 cr3;
   uint32 eip;
   uint32 eflags;
   uint32 eax;
   uint32 ecx;
   uint32 edx;
   uint32 ebx;
   uint32 esp;
   uint32 ebp;
   uint32 esi;
   uint32 edi;
   uint16 es, empty5;
   uint16 cs, empty6;
   uint16 ss, empty7;
   uint16 ds, empty8;
   uint16 fs, empty9;
   uint16 gs, empty10;
   uint16 ldt, empty11;   
  
   uint16 trapflag;
   uint16 iomapbase;

//   uint8  IObitmap[8193];

};

#endif

