#ifndef _KERNEL_i686_INT_H
#define _KERNEL_i686_INT_H

#include <types.h>

// FLAGS
#define IDT_PRESENT  0x8000      //  10000000 00000000b
#define IDT_TRAP     0x0700      //  00000111 00000000b
#define IDT_INT      0x0600      //  00000110 00000000b
#define IDT_TASK     0x0500      //  00000101 00000000b
#define IDT_32       0x0800      //  00001000 00000000b

#define IDT_DPL0     0x0000      //  00000000 00000000b
#define IDT_DPL1     0x2000      //  00100000 00000000b
#define IDT_DPL2     0x4000      //  01000000 00000000b
#define IDT_DPL3     0x6000      //  01100000 00000000b


struct IDT_GATE
{
  uint16 offset15_0;
  uint16 segment;
  uint16 flags;
  uint16 offset16_31;
} __attribute__ ((packed));



/* Function calls */

void init_interrupts();
void set_interrupt(uint16 irq_num, uint16 type, uint16 segment, uint32 offset, uint16 level);


// *************************************************


// *************************************************


/* System Hardware IRQ's  */
 void __null_int();

//
 void __exception_handler0();
 void __exception_handler1();
 void __exception_handler2();
 void __exception_handler3();
 void __exception_handler4();
 void __exception_handler5();
 void __exception_handler6();
 void __exception_handler7();
 void __exception_handler8();
 void __exception_handler9();
 void __exception_handlerA();
 void __exception_handlerB();
 void __exception_handlerC();
 void __exception_handlerD();
 void __exception_handlerE();
 void __exception_handlerF();



//

#endif


