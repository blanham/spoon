#include <types.h>
#include <dmesg.h>
#include <strings.h>
#include <macros.h>
#include <font.h>
#include <lfb.h>
#include "multiboot.h"





static uint32 *lfb = NULL;
static uint32 lfb_screenWidth  = 0;
static uint32 lfb_screenHeight = 0;
static uint32 lfb_screenDepth  = 0;
static uint32 lfb_screenType 	 = 0;
	

void lfb_scroll();
void lfb_clear();
void lfb_putc( int x, int y,  char c );


// ------ forward declarations



void init_lfb(  multiboot_info_t *multiboot_info )
{
	struct vbe_mode *mode_info;
	void *lfb_screenPtr;

	if ( (multiboot_info->flags & (1<<11)) == 0 )
	{
	    lfb_screenWidth  = 0;
	    lfb_screenHeight = 0;
	    lfb_screenDepth = 0;
	    lfb_screenType = 0;
	    lfb_screenPtr = NULL;
		dmesg( "VBE LFB information from multiboot info is not available.\n" );

	}
	else
	{
	    mode_info = (struct vbe_mode*) multiboot_info->vbe_mode_info;
	    lfb_screenWidth  = mode_info->x_resolution;
	    lfb_screenHeight = mode_info->y_resolution;
	    lfb_screenDepth = mode_info->bits_per_pixel;
	    lfb_screenType = 0;
	    lfb_screenPtr = (void*)mode_info->phys_base;

		lfb = (uint32*)lfb_screenPtr;
	
		dmesg( "%s %i x %i x %i at lfb %x\n",
					"VBE LFB information: ",
					lfb_screenWidth,
					lfb_screenHeight,
					lfb_screenDepth,
					lfb_screenPtr );
     
	}

		
	set_lfb( lfb_screenWidth, lfb_screenHeight, lfb_screenDepth, lfb_screenType, lfb_screenPtr );
}


// -------------------------------------





int  lfb_width()
{
	return (lfb_screenWidth / 8);
}


int  lfb_height()
{
	return (lfb_screenHeight / 16);
}


/// \todo remove the remove warning after debugging.
//#warning remove me after debugging
#include <paging.h>
#include "paging.h"

void lfb_clear()
{
  int bytes;
  
  //#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
 

	bytes = lfb_screenWidth * lfb_screenHeight;
	memset( lfb, 0, bytes * 4 ); 
  
   set_vmmap( &sam );
}


void lfb_scroll()
{
	int bytes;


//#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
 

	bytes = lfb_screenWidth * lfb_screenHeight - (16 * lfb_screenWidth);
					// Total amount of bytes to copy.
	
	memcpy( (void*)lfb, 
			(void*)((unsigned int)lfb + (16 * lfb_screenWidth * 4)), 
			bytes * 4 );
	
	memset( (void*)((unsigned int)lfb + bytes * 4), 
			0,
			16 * lfb_screenWidth * 4 );

   set_vmmap( &sam );
}





void lfb_putc( int x, int y, char c)
{
  int i, j;
  int pix;
  int subscr = c;


//#warning and remove me
   vmmap sam;
   get_vmmap( &sam );
   vmmap bob;
   		 bob.pde = &page_directory;
		 set_vmmap( &bob );
  
			for ( i = 0; i < 8; i++ )
			 for ( j = 0; j < 16; j++ )
			 {
				pix = x * 8 + i + (y * 16 + j) * lfb_screenWidth;
				
				if ( font[subscr][j][i] == 1 ) lfb[ pix ] = 0xFFFFFF;
							         else lfb[ pix ] = 0x0;
			 }

//#warning remove me too
	set_vmmap( &sam );
}



