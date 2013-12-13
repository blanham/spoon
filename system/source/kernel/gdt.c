#include <gdt.h>
#include <tss.h>
#include <inttypes.h>
#include <dmesg.h>
#include <tss.h>
#include <assert.h>

#include <cpu.h>

#include <physmem.h>


#define GDTENTRY( base, length, flags, type, access )		\
		{ (uint16_t)(length & 0xFFFF),						\
		  (uint16_t)(base & 0xFFFF),						\
		  (uint8_t)((base>>16) & 0xFF),						\
		  (flags | access),									\
		  (type | ((length >> 16) & 0xF)),					\
		  (uint8_t)((base >> 24) & 0xF)						\
		}


/** The temporary GDT table for AP processors */
segment_descriptor tempGDT[3] __attribute__ ((aligned (4096))) 
		= { 
			GDTENTRY(0,0,0,0,0),
			GDTENTRY(0, 0xFFFFFF,  
				 G_CODE |  G_PRESENT | G_READABLE | G_APPLICATION,  
			     G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL0) ,
			GDTENTRY(0, 0xFFFFFF,  
				  G_DATA |  G_PRESENT | G_WRITABLE | G_APPLICATION,  
			      G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL0),
			};
	



void init_gdt()
{

	// Set up the tempGDT NULL descriptor accurately for the AP's. 
	*(uint16_t*)( tempGDT ) 			= 0xFFFF;
	*(uint32_t*)( (void*)tempGDT + 2 )  = (uint32_t)tempGDT;
	*(uint16_t*)( (void*)tempGDT + 6 )	= 0;

}


static void gdt_updateSegments(unsigned int new_code, unsigned int new_data)
{
  asm (
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




/** CPU specific GDT allocation, manipulation and initialization */
int load_gdt( unsigned int cpu_id )
{
	int system_entry, boot_entry;
	uint32_t gdt_Location[2];
	struct TSS* boot_tss;
	struct TSS* system_tss;
    void *gdt_Ptr;
	int i;

	
	// Allocate the GDT
	cpu[ cpu_id ].gdt = 
			(segment_descriptor*)
				memory_alloc( (sizeof(segment_descriptor)*MAX_GDT)/4096 + 1 );
	
     for (i = 0; i < MAX_GDT; i++)  
     {
		cpu[ cpu_id ].gdt[i].seg_length0_15 = 0;
		cpu[ cpu_id ].gdt[i].base_addr0_15  = 0; 
		cpu[ cpu_id ].gdt[i].base_addr16_23 = 0; 
		cpu[ cpu_id ].gdt[i].flags = 0; 
		cpu[ cpu_id ].gdt[i].type = 0;
		cpu[ cpu_id ].gdt[i].base_addr24_31 = 0;
     }

 
	gdt_new_segment( cpu_id, 0, 0xFFFFFF,  
					 G_CODE |  G_PRESENT | G_READABLE | G_APPLICATION,  
				     G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL0);
	gdt_new_segment( cpu_id, 0, 0xFFFFFF,  
					  G_DATA |  G_PRESENT | G_WRITABLE | G_APPLICATION,  
				      G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL0);
	gdt_new_segment( cpu_id, 0, 0xFFFFFF,  
					 G_CODE |  G_PRESENT | G_READABLE | G_APPLICATION,  
				     G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL3);
	gdt_new_segment( cpu_id, 0, 0xFFFFFF,  
					  G_DATA |  G_PRESENT | G_WRITABLE | G_APPLICATION,  
				      G_USE32 | G_AVAIL | G_GRANULAR ,  G_DPL3);


	boot_tss   = new_cpu_tss(0);	// To be deleted later.
	system_tss = new_cpu_tss(1);
	
    boot_entry = gdt_new_segment( cpu_id, (uint32_t)(boot_tss), 
									sizeof(struct TSS),
                                    G_TSS32 | G_SYSTEM | G_PRESENT, 
                                    G_AVAIL,  
									G_DPL0 );
	
	
	system_entry = gdt_new_segment( cpu_id, (uint32_t)(system_tss), 
									sizeof(struct TSS),
         		                    G_TSS32 | G_SYSTEM | G_PRESENT, 
                               		G_AVAIL,  
									G_DPL3 );


	// Now let's do the loading... 
	
	gdt_Location[0] = 0xffff << 16;
	gdt_Location[1] = (uint32_t)( cpu[ cpu_id ].gdt );
 	gdt_Ptr = (void*)gdt_Location + 2;

  // load the gdt, using the NULL entry.
    asm ("lgdt (%0)": :"p" (gdt_Ptr));
	
	
  // update the segments on the cpu
    gdt_updateSegments( GDT_SYSTEMCODE, GDT_SYSTEMDATA );


  // Load the boot TSS into the TR.
    asm volatile ( " ltr %0 " : : "g" (boot_entry) );


	dmesg("CPU %i gdt loaded. boot #%i, system #%i.\n", 
						cpu_id, 
						boot_entry / 8 ,
						system_entry / 8 );

	cpu[ cpu_id ].gdt_boot = boot_entry;
	cpu[ cpu_id ].gdt_system = system_entry;
	cpu[ cpu_id ].system_tss = system_tss;
	return 0;
}





int gdt_new_segment( unsigned int id,
					  uint32_t base, 
					  uint32_t length, 
					  uint8_t flags, 
					  uint8_t type, 
					  uint32_t access)
{
	int pos;

		for ( pos = 1; pos < MAX_GDT; pos++ )
		{
			if ( cpu[id].gdt[pos].type == 0 )
			{
				cpu[id].gdt[pos].seg_length0_15 = (uint16_t)(length & 0xFFFF);
				cpu[id].gdt[pos].base_addr0_15 = (uint16_t)(base & 0xFFFF);
				cpu[id].gdt[pos].base_addr16_23 = (uint8_t)((base>>16) & 0xFF);
				cpu[id].gdt[pos].flags = flags | access;
				cpu[id].gdt[pos].type = type | ((length >> 16) & 0xf);
				cpu[id].gdt[pos].base_addr24_31 = (uint8_t)((base>>24)&0xF);
				return pos * 8;
			}
		}

	assert( 1 == 2 );
	return -1;
}



uint32_t gdt_get_offset( unsigned int cpu_id, unsigned int entry )
{
	uint32_t offset;

		offset  = cpu[ cpu_id ].gdt[entry/8].base_addr0_15;
	    offset += cpu[ cpu_id ].gdt[entry/8].base_addr16_23 << 16;
		offset += cpu[ cpu_id ].gdt[entry/8].base_addr24_31 << 24;

	return offset;
}


void call_task( uint32_t gdt )
{
   uint32_t sel[2];

      sel[0] = 0xDEAD;
      sel[1] = gdt;
	  
     asm ( "lcall *(%0)": :"g" ((uint32_t*)&sel) ) ;
}

void jump_task( uint32_t gdt )
{
   uint32_t sel[2];

      sel[0] = 0xDEAD;
      sel[1] = gdt;
	  
     asm ("ljmp *(%0)": :"g" ((uint32_t*)&sel) );
}


