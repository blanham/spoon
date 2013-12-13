#ifndef _KERNEL_i686_GDT_H
#define _KERNEL_i686_GDT_H

#include <types.h>

// DPL level. USER = 3, SYSTEM = 0;
#define G_DPL3        0x60 //1100000 
#define G_DPL1        0x20 //0100000
#define G_DPL2        0x40 //1000000
#define G_DPL0        0x00 //0000000  

// Type:  My normal code = G_USE32 | G_AVAIL
#define G_USE32       0x40 //01000000
#define G_USE16       0x00 //00000000
#define G_GRANULAR    0x80 //10000000
#define G_AVAIL       0x10 //00010000 

// Flags: My normal code = G_CODE |  G_PRESENT | G_READABLE | G_APPLICATION
#define G_SYSTEM       0x00 //00000  TSS. LDT. int. whatever
#define G_APPLICATION  0x10 //10000  application code


// FLAGS -- type of G_SYSTEM segments
#define G_TSS16        0x1  // 0001 16 bit TSS (Available)
#define G_LDT          0x2  // 0010 LDT
#define G_TSS16_BUSY   0x3  // 0011 16 bit TSS (Busy)
#define G_CALL16       0x4  // 0100 16 bit call gate
#define G_TASK         0x5  // 0101 Task gate
#define G_INT16        0x6  // 0110 16 bit interrupt gate
#define G_TRAP16       0x7  // 0111 16 bit trap gate
#define G_TSS32        0x9  // 1001 32 bit TSS (available)
#define G_TSS32_BUSY   0xB  // 1011 32 bit TSS (busy)
#define G_CALL32       0xC  // 1100 32 bit call gate
#define G_INT32        0xE  // 1110 32 bit interrupt gate
#define G_TASK_GATE    0xF  // 1111 32 bit trap gate

// FLAGS -- type of G_APPLICATION segments
#define G_CODE         0x08 //01000
#define G_CONFORMING   0x04 //00100   (only code)
#define G_READABLE     0x02 //00010   (only code)

#define G_DATA         0x00 //00000
#define G_EXPAND_DOWN  0x04 //00100   (only data)
#define G_WRITABLE     0x02 //00010   (only data)

#define G_ACCESSED     0x01 //00001      (both)
#define G_PRESENT      0x80 //010000000  (both. use for virtual paging & what not)

typedef struct 
{
	uint16 seg_length0_15;
	uint16 base_addr0_15; 
	uint8 base_addr16_23; 
	uint8 flags; 
	uint8 type;
	uint8 base_addr24_31;
} segment_descriptor ;

extern spinlock spinlock_gdt;

extern uint16 gdt_system_code_segment;
extern uint16 gdt_system_data_segment;
extern uint16 gdt_user_code_segment;
extern uint16 gdt_user_data_segment;




// **************************************************
uint16 user_CS();  
uint16 user_DS();  
uint16 system_CS();
uint16 system_DS();
void* gdt_location();
// **************************************************


// updated GDT functions

void gdt_init();
void gdt_updateSegments(uint32 new_code, uint32 new_data);

uint32 gdt_new_segment( uint32 base, uint32 length, uint8 flags, uint8 type, uint32 access);
  void gdt_del_segment( uint32 entry );

uint32 gdt_get_dpl( uint32 entry );
  void gdt_set_dpl( uint32 entry, uint32 dpl );
  
uint32 gdt_get_offset( uint32 entry );
  void gdt_set_offset( uint32 entry, uint32 offset );
  
uint8 gdt_get_flags( uint32 entry );
 void gdt_set_flags( uint32 entry, uint8 new_flags );


#endif
