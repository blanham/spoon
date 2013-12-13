#include <atomic.h>
#include <dmesg.h>

#include "gdt.h"

static uint16 gdt_max_entry = 50;
static segment_descriptor gdt_Table[50];

spinlock  spinlock_gdt = INIT_SPINLOCK;

uint16 gdt_system_code_segment;
uint16 gdt_system_data_segment;
uint16 gdt_user_code_segment;
uint16 gdt_user_data_segment;

uint16 user_CS()   { return gdt_user_code_segment; }
uint16 user_DS()   { return gdt_user_data_segment; }
uint16 system_CS() { return gdt_system_code_segment; }
uint16 system_DS() { return gdt_system_data_segment; }
void* gdt_location() { return &gdt_Table; }


void gdt_updateSegments(uint32 new_code, uint32 new_data)
{
  __asm__ __volatile__ (
               "\n"
	         "pushl %0\n"
	         "pushl $1f\n"
             "lret \n"
		"1:\n"
		 "mov %1, %%eax\n"
		 "mov %%ax, %%ds\n"
		 "mov %%ax, %%es\n"
		 "mov %%ax, %%fs\n"
		 "mov %%ax, %%gs\n"
		 "mov %%ax, %%ss\n"
	       
	       : 
	       : "g" (new_code), "g" (new_data)
	       : "eax");
}


void gdt_init()
{
  uint32 tmp_val[2];
      char *the_table = (char*)&gdt_Table;

  uint16* gdt_size    = (uint16*)&gdt_Table;
  uint32* gdt_pointer = (uint32*)( &(the_table[2]) );
  uint16* gdk_zero    = (uint16*)( &(the_table[6]) );
  uint32 i;

  // First clear the table
     for (i = 0; i < gdt_max_entry; i++)  
     {
       gdt_Table[i].seg_length0_15 = 0;
       gdt_Table[i].base_addr0_15  = 0; 
       gdt_Table[i].base_addr16_23 = 0; 
       gdt_Table[i].flags = 0; 
       gdt_Table[i].type = 0;
       gdt_Table[i].base_addr24_31 = 0;
     }

  // set the gdt null descriptor correctly
         *gdt_size = 0xffff;
         *gdt_pointer =  (uint32)( &gdt_Table );
         *gdk_zero    = 0;

  // set up some the four main segments

    gdt_system_code_segment = gdt_new_segment(0, 
		                              0xFFFFFF,  
					      G_CODE |  G_PRESENT | G_READABLE | G_APPLICATION,  
					      G_USE32 | G_AVAIL | G_GRANULAR ,  
					      G_DPL0);
    gdt_system_data_segment = gdt_new_segment(0, 
		                              0xFFFFFF,  
					      G_DATA |  G_PRESENT | G_WRITABLE | G_APPLICATION,  
					      G_USE32 | G_AVAIL | G_GRANULAR ,  
					      G_DPL0);
   gdt_user_code_segment = gdt_new_segment(0, 
		                           0xFFFFFF,  
					   G_CODE | G_PRESENT | G_READABLE | G_APPLICATION/*|G_CONFORMING*/, 
					   G_USE32 | G_AVAIL | G_GRANULAR,  
					   G_DPL3);
   gdt_user_data_segment = gdt_new_segment(0, 
		                           0xFFFFFF,  
					   G_DATA |  G_PRESENT | G_WRITABLE | G_APPLICATION,  
					   G_USE32 | G_AVAIL | G_GRANULAR ,  
					   G_DPL3);

  // load the gdt

    tmp_val[0] = 0xffff << 16;
    tmp_val[1] = (uint32)&gdt_Table;
    __asm__ __volatile__ ("lgdt (%0)": :"g" (((char *)tmp_val )+2));

  // update the segments on the cpu
  gdt_updateSegments( gdt_system_code_segment, gdt_system_data_segment );
}


void gdt_del_segment( uint32 entry )
{

	acquire_spinlock( &spinlock_gdt );
	
  gdt_Table[entry/8].seg_length0_15 = 0;
  gdt_Table[entry/8].base_addr0_15  = 0; 
  gdt_Table[entry/8].base_addr16_23 = 0; 
  gdt_Table[entry/8].flags = 0; 
  gdt_Table[entry/8].type = 0;
  gdt_Table[entry/8].base_addr24_31 = 0;

	release_spinlock( &spinlock_gdt );
}


uint32 gdt_new_segment( uint32 base, uint32 length, uint8 flags, uint8 type, uint32 access)
{
  uint16 num = 1;
  num = 1;
  
	acquire_spinlock( &spinlock_gdt );
	
  while ( num < gdt_max_entry )
  {
    if (
         (gdt_Table[num].seg_length0_15 == 0) &&
         (gdt_Table[num].base_addr0_15  == 0) &&
         (gdt_Table[num].base_addr16_23 == 0) &&
         (gdt_Table[num].flags == 0) &&
         (gdt_Table[num].type == 0) &&
         (gdt_Table[num].base_addr24_31 == 0)
       )
    {
      gdt_Table[num].seg_length0_15 = (uint16)(length & 0xFFFF);
      gdt_Table[num].base_addr0_15 = (uint16)(base & 0xFFFF);
      gdt_Table[num].base_addr16_23 = (uint8)((base >> 16) & 0xFF);
      gdt_Table[num].flags = flags | access;
      gdt_Table[num].type = type | ((length >> 16) & 0xf);
      gdt_Table[num].base_addr24_31 = (uint8)((base & 0xF000) >> 24);

	  release_spinlock( &spinlock_gdt );
      return (num * 8);
    }
    num++;
  }


  dmesg( "%!%s\n", "GDT IS FULL!!!! This is bad. Very bad. I never thought this could happen. You should freak out now.");
  while (1==1) {};

  release_spinlock( &spinlock_gdt );
 
  return 0;
}


uint32 gdt_get_offset( uint32 entry )
{
	uint32 offset;

   acquire_spinlock( &spinlock_gdt );
	
	offset = 0;
	offset = gdt_Table[entry/8].base_addr0_15;
        offset = offset + (gdt_Table[entry/8].base_addr16_23 << 16);
	offset = offset + (gdt_Table[entry/8].base_addr24_31 << 24);

   release_spinlock( &spinlock_gdt );
	
	return offset;
}

void gdt_set_offset( uint32 entry, uint32 offset )
{
   acquire_spinlock( &spinlock_gdt );
      gdt_Table[entry/8].base_addr0_15 = (uint16)(offset & 0xFFFF);
      gdt_Table[entry/8].base_addr16_23 = (uint8)((offset >> 16) & 0xFF);
      gdt_Table[entry/8].base_addr24_31 = (uint8)((offset & 0xF000) >> 24); 
   release_spinlock( &spinlock_gdt );
}


uint32 gdt_get_dpl( uint32 entry )
{
	uint32 dpl;
	acquire_spinlock( &spinlock_gdt );
        dpl = (gdt_Table[entry/8].flags & G_DPL3);
	release_spinlock( &spinlock_gdt );
	return dpl;
}

void gdt_set_dpl( uint32 entry, uint32 dpl)
{
   uint8 old_flags;

	acquire_spinlock( &spinlock_gdt );
         old_flags = gdt_Table[entry/8].flags;
         old_flags = old_flags & ~(G_DPL3);
   gdt_Table[entry/8].flags = old_flags | dpl;
	release_spinlock( &spinlock_gdt );
}

uint8 gdt_get_flags( uint32 entry ) 
{ 
	uint8 flags;
	acquire_spinlock( &spinlock_gdt );
	flags =  gdt_Table[entry/8].flags; 
	release_spinlock( &spinlock_gdt );
	return flags;
}

void gdt_set_flags( uint32 entry, uint8 new_flags)
{
   uint8 old_flags;
	acquire_spinlock( &spinlock_gdt );
         old_flags = gdt_Table[entry/8].flags;
         old_flags = old_flags & (G_DPL3);
   gdt_Table[entry/8].flags = new_flags | old_flags;
	release_spinlock( &spinlock_gdt );
}


