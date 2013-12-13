#include <types.h>
#include <macros.h>
#include <dmesg.h>
#include <interrupts.h>
#include <process.h>
#include <scheduler.h>

#include "misc.h"
#include "io.h"
#include "process_data.h"

#define TICKS_PER_SECOND	10
#define HZ					100
#define FREQUENCY           (0x1234DD / TICKS_PER_SECOND) 
#define SECONDS_PER_DAY		86400

//  CPU timing code by giszo <king@startadsl.hu> 



// ----- GLOBAL VARIABLES --------------------------------

uint32 cpu_speed;			//  CPU speed in MHz
uint32 bogomips;			//  Operations per millisecond.
uint32 system_seconds;		//
uint32 system_milliseconds;	//

uint32 system_bogs;
uint64 last_rdtsc;

time_t boot_time;


// --- forward declarations ---

unsigned long 	calculate_bogomips();
time_t 			get_rtc_seconds();
uint64 			rdtsc();

unsigned long 	time_cpu();
void 			init_pit();

// ----------------------------


void init_time()
{
  
  init_pit();
		
  boot_time = get_rtc_seconds();
   bogomips = calculate_bogomips();
  cpu_speed = bogomips / 1000;

   dmesg("%s%i%s%i%s\n",
				"CPU: ",cpu_speed,
				" MHz, ", bogomips,
				" operations per millisecond.");
	
  if ( cpu_speed == 0 )
  {
	dmesg("%!%s\n","CPU speed is reported as 0 MHz. Please let me know and send");
	dmesg("%!%s\n","your system specs too. ");
	while (1==1) {}
  }
  
	  system_seconds = boot_time;
	  system_milliseconds = 0;
	  system_bogs = 0;

	  last_rdtsc = rdtsc();
}


void init_pit()
{
  uint8 high, low;

  dmesg("initializing system timer.\n");
  dmesg("modifying PIT clock frequency.\n"); 

  high = (FREQUENCY / 256) & 0xFF;
   low = (FREQUENCY % 256) & 0xFF;

  outb( 0x43, 0x34 );  // Select channel 0 using the control port
  outb( 0x40, low);
  outb( 0x40, high); 
}



// -----------------------------------------------------------------
// ******  TIMING CODE *********************************************  

#define LATCH ( ( 1193180 + HZ / 2 ) / HZ )
#define CALIBRATE_LATCH ( 5 * LATCH )
#define CALIBRATE_TIME ( 5 * 1000020 / HZ )


inline void get_rdtsc( unsigned long *low, unsigned long *high )
{
  asm volatile( "rdtsc" : "=a" ( *low ), "=d" ( *high ) );
}

unsigned long calibrate_tsc()
{
  unsigned long startlow, starthigh, endlow, endhigh, count;
    
  outb( 0x61, ( inb( 0x61 ) & ~0x02 ) | 0x01 );
  outb( 0x43, 0xb0 );
  outb( 0x42, CALIBRATE_LATCH & 0xff );
  outb( 0x42, (CALIBRATE_LATCH >> 8) & 0xff );
  get_rdtsc( &startlow, &starthigh );
  count = 0;
  do
  {
    count++;
  } while ( ( inb( 0x61 ) & 0x20 ) == 0 );
  
  get_rdtsc( &endlow, &endhigh );
  
  if ( count <= 1 ) return 0;
  
  asm( "subl %2, %0\n"
       "sbbl %3,%1" 
	  : "=a" ( endlow ), 
	    "=d" ( endhigh ) 
	  : "g" ( startlow ), 
	    "g" ( starthigh ), 
		"0" ( endlow ), 
		"1" ( endhigh ) );
  
  if ( endhigh != 0 ) return 0;
  if ( endlow <= CALIBRATE_TIME ) return 0;
  
  asm ( "divl %2" 
		: "=a" ( endlow ), 
		  "=d" ( endhigh ) 
		: "r" ( endlow ),
		  "0" ( 0 ), 
		  "1" ( CALIBRATE_TIME ) );
  
  return endlow;
}    

// Get the CPU speed
unsigned long calculate_bogomips()
{
  unsigned long tsc_quotient = calibrate_tsc();
  unsigned long cpu_hz = 0;

  if ( tsc_quotient != 0 )
  {
    unsigned long eax = 0, edx = 1000000;
    asm( "divl %2" 
		: "=a" ( cpu_hz ), 
		  "=d" ( edx ) 
		: "r" ( tsc_quotient ), 
		  "0" ( eax ), 
		  "1" ( edx ) );
  }
  return cpu_hz / 1000;
}


// --------------------------------------------------------------------


uint32 get_rtc_seconds()
{
  uint32 seconds, minutes, hours;
  uint32 total;
  uint32 flags;

  flags = cpu_flags();
  disable_interrupts();

	  do  // make sure UIP is not set. (bit 7, timer is being updated)
	  {
	    outb( 0x70, 0xA );
	  } while ( (inb(0x71) & (1<<7)) != 0);
	
	
	  // check CMOS
	
	  outb( 0x70, 0x0);
	  seconds = inb( 0x71 );
	
	  outb( 0x70, 0x2);
	  minutes = inb( 0x71 );
	
	  outb( 0x70, 0x4 );
	  hours = inb( 0x71 );
	
	    hours = (hours   >> 4) * 10 + (hours & 0xF);
	  seconds = (seconds >> 4) * 10 + (seconds & 0xF);
	  minutes = (minutes >> 4) * 10 + (minutes & 0xF);  
	  
	  total = hours * 60 * 60 + minutes * 60 + seconds;


  set_cpu_flags( flags );

  return total;
}


uint64 rdtsc(void)
{
   uint64 x;
   __asm__ volatile(".byte 0x0f,0x31" : "=A" (x));
   return x;
}




time_t epoch_seconds()
{
	return get_rtc_seconds();
}

time_t epoch_uptime()
{
	return (epoch_seconds() - boot_time);
}

unsigned int get_cpu_speed()
{
	return cpu_speed;
}

int system_time( time_t *seconds, time_t *milliseconds )
{
	*seconds = system_seconds;
	*milliseconds = system_milliseconds;
	return 0;
}

int delay( unsigned int milliseconds )
{
	uint32 seconds_big;
	uint32 milli_big;

	// Calculate the time (now + milliseconds) in the future

	milli_big = system_milliseconds + milliseconds;
	seconds_big = system_seconds + (milli_big / 1000);
	milli_big = milli_big % 1000;

	// Wait for it.

	while ((  system_seconds < seconds_big   )) sched_yield();
	while (( system_milliseconds < milli_big ) &&
	       ( system_seconds == seconds_big   )) sched_yield();
	
	return 0;
}




int increment_system_time( struct process *proc )
{
	uint64 diff;
	uint64 temp;
	uint32 mips;
	struct process_data *data = NULL;

	temp = rdtsc();
	diff = temp - last_rdtsc;
	last_rdtsc = temp;

	mips = (uint32)diff;	// The number of operations since the last update.

		// update process time usage --------------------

			data = (struct process_data*)proc->data;
			
			data->bogomips += mips;

			if ( data->bogomips >= bogomips )
			{
				proc->usage_milliseconds += data->bogomips / bogomips;
						   data->bogomips = data->bogomips % bogomips;
				
				if ( proc->usage_milliseconds >= 1000 )
				{
					proc->usage_seconds += proc->usage_milliseconds / 1000;
					proc->usage_milliseconds = proc->usage_milliseconds % 1000;
				}
			}
			
		// update system time ---------------------------

   	system_bogs += mips;
			
	if ( system_bogs >= bogomips )
	{
		system_milliseconds += system_bogs / bogomips;
				 system_bogs = system_bogs % bogomips;

		if ( system_milliseconds >= 1000 )
		{
			system_seconds     += system_milliseconds / 1000;
			system_milliseconds = system_milliseconds % 1000;
		}
	}

	return 0;
}



