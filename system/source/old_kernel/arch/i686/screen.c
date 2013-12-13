#include <types.h>


#define CON_SCREEN		0xB8000
#define SCREEN_ATTR		7
#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25

static char *screen = (char *) CON_SCREEN;

// -------------------------------------

int  screen_width()
{
	return SCREEN_WIDTH;
}

int  screen_height()
{
	return SCREEN_HEIGHT;
}


#include <paging.h>
#include "paging.h"

void screen_scroll()
{
    int i,j;

	//#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
 
	
	for ( i = 0; i < (SCREEN_HEIGHT - 1); i++ )
	{
		for ( j = 0; j < SCREEN_WIDTH * 2; j++ )
		{
			screen[ i * SCREEN_WIDTH * 2 + j ] = screen[ (i + 1) * SCREEN_WIDTH * 2 + j ];
		}
	}
	

	for ( j = 0; j < SCREEN_WIDTH; j++ )
	{
		screen[ (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2 + j ] = ' ';
		screen[ (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2 + j + 1 ] = SCREEN_ATTR;
	}

//#warning remove me too
	set_vmmap( &sam );
}


void screen_clear(void) 
{
  int i,j;
  //
//#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
 

     for( i = 0; i < SCREEN_HEIGHT; i++)
      for( j = 0; j < SCREEN_WIDTH; j++ )
      {
        screen[i * SCREEN_WIDTH * 2 + j * 2] = ' ';
        screen[i * SCREEN_WIDTH * 2 + j * 2 +1] = SCREEN_ATTR;	
      }

//#warning remove me too
	set_vmmap( &sam );
}


void screen_putc(int x, int y, char c)
{
//#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
 
		
    screen[ y * SCREEN_WIDTH * 2 + x * 2] = c;
    screen[ y * SCREEN_WIDTH * 2 + x * 2 + 1 ] = SCREEN_ATTR;

//#warning remove me too
	set_vmmap( &sam );
}


