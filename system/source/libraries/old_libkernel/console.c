#include <types.h>
#include <kernel.h>

// Console information 


/**  \defgroup CONSOLE  Console Operations  

<P>
The kernel contains several text consoles.  Several text consoles can
be attached to a single text console.

<P>
Technically speaking, the kernel console structure contains a screen 
buffer and a data buffer.  The screen buffer is a temporary page of memory
into which the processes write while the particular console is
not active - being displayed on the screen.
<P>
The data buffer is another page of memory which is mapped into
all the processes which are attached to the particular console.
<P>
Each process has the screen buffer ( or screen if it's the active
console) mapped in as read-write, it's console's data buffer mapped
in as read-write and the active console's data buffer mapped in
as read-only.
<P>
It's been done this way in order to be able to provide text and
console synchronisation between the processes on each console.
Also, the active console is also available in order to be
able to provide global information about which processes are
the active processes - receiving human input.


  
 */

/** @{ */


#define CONSOLE_MAGIC	0xC007

struct console_data
{
  uint32 magic;				// magic number
  uint32 spinlock;			// console spinlock
  uint32 mode;				// console mode;
  int pid;				// pid for MESSAGE
  int port;				// port for MESSAGE
  uint8 x;				// cursor x
  uint8 y;				// cursor y
  uint32 attr;				// colours
} __attribute__ ((packed));

static char *console = NULL;		  // actual screen
static int32 console_size;		  // size of screen

static struct console_data *console_ld = NULL;	  // local console data

/*

   The functions which directly print information to the screen are:

	conio_clear
	conio_putc


*/


// kernel console manipulation - set & get

int32 smk_set_console( int32 console )
{
	return _sysenter( (SYS_CONSOLE|SYS_THREE), console, 0, 0,0,0 );
}

int32 smk_get_console( int pid )
{
	return _sysenter( (SYS_CONSOLE|SYS_FOUR), pid, 0, 0,0,0 );
}

char *smk_get_console_location()
{
	return (char*)_sysenter((SYS_CONSOLE|SYS_ONE),0,0,0,0,0);
}

int smk_switch_console( int pid, int console )
{
	return _sysenter( (SYS_CONSOLE|SYS_FIVE), pid, console, 0,0,0 );
}

// ------ global data reading

int smk_active_pid( int id )
{
	return _sysenter( (SYS_CONSOLE|SYS_EIGHT), 1, id,  0,0,0 );
}

int smk_set_active_pid( int id, int pid )
{
	return _sysenter( (SYS_CONSOLE|SYS_EIGHT), 0, id,  pid ,0,0 );
}

// ------ Initialisation

void smk_init_console()
{
	int i;
	
	console = (char*)_sysenter((SYS_CONSOLE|SYS_ONE),0,0,0,0,0);
	console_size = (int32)_sysenter((SYS_CONSOLE|SYS_TWO),0,0,0,0,0);

			// Last page of console.
	console_ld = (struct console_data*)
			( (uint32)console + console_size - 4096 );

	if ( console_ld->magic != CONSOLE_MAGIC )
	{
	   console_ld->magic = CONSOLE_MAGIC;
	   console_ld->spinlock = 1;
	   console_ld->mode = VC_MODE_BUFFER;
	   console_ld->x = 0;
	   console_ld->y = 0;
	   console_ld->pid  = -1;
	   console_ld->port = -1;
	   
	   console_ld->attr = 7;

	   for ( i = 0; i < (160*25); i+=2) 
	   {
	 	console[i] = 0;
	 	console[i+1] = console_ld->attr;
	   }

	   smk_release_spinlock( & (console_ld->spinlock) );
	}

}


// -----------------------------------------

int smk_new_console( int pages )
{
  return _sysenter((SYS_CONSOLE|SYS_SIX),0,pages,0,0,0);
}

int smk_stage_console( void* data, int pages )
{
  return _sysenter((SYS_CONSOLE|SYS_SIX),1,(uint32)data,pages,0,0);
}

int smk_delete_console( int id )
{
  return _sysenter((SYS_CONSOLE|SYS_SEVEN),id,0,0,0,0);
}

// -----------------------------------------

int smk_create_console( int pages, int mode, int pid, int port, int active_pid )
{
  struct console_data *rConsole;
  void *data;
  char *buf;
  int i;

  if ( pages < 3 ) return -1;

  data = smk_mem_alloc( pages ); 
  
  buf = (char*)data;
  for ( i = 0; i < (pages * 4096); i += 2 )
  {
   buf[ i ] = ' ';
   buf[ i + 1 ] = 7;
  }

  

  rConsole = (struct console_data*)( (uint32)data + (pages-1) * 4096 );

	   rConsole->magic = CONSOLE_MAGIC;
	   rConsole->spinlock = 0;
	   rConsole->mode = mode;
	   rConsole->x = 0;
	   rConsole->y = 0;
	   rConsole->pid  = pid;
	   rConsole->port = port;
	   
	   rConsole->attr = 14;

  i = smk_stage_console( data, pages );

  if ( i >= 0 ) smk_set_active_pid( i, active_pid );

  return i;
}

// --------- actual console stuff.

void smk_setfgbg( int fg, int bg )
{
   smk_acquire_spinlock( & (console_ld->spinlock) );
	console_ld->attr = (( bg & 0xF ) << 4) + ( fg & 0xF );
   smk_release_spinlock( & (console_ld->spinlock) );
}

void smk_conio_gotoxy( uint8 x, uint8 y)
{
	if ( x > 80 ) return;
	if ( y > 24 ) return;
   
   smk_acquire_spinlock( & (console_ld->spinlock) );
	console_ld->x = x;
	console_ld->y = y;

	if ( console_ld->mode == VC_MODE_MESSAGE )
	 {
	   smk_send_pulse( 0, console_ld->pid, console_ld->port,
	   	       VC_GOTOXY, x, y, 0,0,0 );
	 }

	
   smk_release_spinlock( & (console_ld->spinlock) );
}


void smk_conio_getxy( uint8 *x, uint8 *y)
{
   smk_acquire_spinlock( & (console_ld->spinlock) );
	*x = console_ld->x;
	*y = console_ld->y;
   smk_release_spinlock( & (console_ld->spinlock) );
}



void smk_conio_clear()
{
	int i;
   smk_acquire_spinlock( & (console_ld->spinlock) );

	if ( console_ld->mode == VC_MODE_MESSAGE )
	 {
	   smk_send_pulse( 0, console_ld->pid, console_ld->port,
	   	       VC_CLEAR, console_ld->attr , 0, 0,0,0 );
           smk_release_spinlock( & (console_ld->spinlock) );
	   return;
	 }


	console_ld->x = 0;
	console_ld->y = 0;

	for ( i = 0; i < (160 * 25); i+=2)
	{
		console[ i ] = 0;
		console[ i + 1 ] = console_ld->attr;
	}

   smk_release_spinlock( & (console_ld->spinlock) );
}



// ---------------- BUFFER MANIPULATION -------------

void smk_conio_putc( unsigned char c )
{
	int i;

	if (console == NULL ) return;

   smk_acquire_spinlock( & (console_ld->spinlock) );

     if ( console_ld->mode == VC_MODE_MESSAGE )
	 {
	   smk_send_pulse( 0, console_ld->pid, console_ld->port,
	   	       VC_PUTC,c,console_ld->attr,0,0,0 );
	   smk_release_spinlock( & (console_ld->spinlock) );
	   return;
	 }
	   
   
   
	if ( (c == '\n') )
	{
		console_ld->x = 0;
		console_ld->y = console_ld->y + 1;
	}
	else
	if ( (c == '\b') )
	{
	   console_ld->x = console_ld->x - 1;
	   if ( console_ld->x > 80 )
	   {
	   	console_ld->x = 79;
		console_ld->y--;
	   }

	   if ( console_ld->y > 25 )
	   {
	   	console_ld->x = 0;
		console_ld->y = 0;
	   }
	}
	else
	{
	
		console[ console_ld->y * 160 + console_ld->x * 2 ] = c;
		console[ console_ld->y * 160 + console_ld->x * 2 + 1] = console_ld->attr;
	
		console_ld->x = console_ld->x + 1;
	}

	if ( console_ld->x == 80 ) 
	{
		console_ld->x = 0;
		console_ld->y = console_ld->y + 1;
	}

	if ( console_ld->y == 25 )
	{
		kmemcpy( console, console + 160, (160*25) - 160 );
	
		for ( i = 0; i < 160; i+=2)
		{
			console[ (160 * 24) + i ] = 0;
			console[ (160 * 24) + i + 1 ] = console_ld->attr;
		}

		console_ld->y = 24;
	}
	
   smk_release_spinlock( & (console_ld->spinlock) );
}


void  smk_conio_puts( const char *n )
{
   int i = 0;
   while ( n[i] != 0 ) smk_conio_putc( n[i++] );
}


/** @} */


