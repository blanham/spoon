#include <configure.h>
#include <types.h>
#include <process.h>
#include <paging.h>

/// \todo Make this static again
uint32 lfb_screenWidth	= 0;
static uint32 lfb_screenHeight= 0;
static uint32 lfb_screenDepth	= 0;
static uint32 lfb_screenType 	= 0;
void* 		lfb_screenPtr 	= NULL;


int using_lfb()
{
	if ( lfb_screenPtr != 0 ) return 1;
	return 0;
}



void do_lfb()
{
	uint32 x;
	uint32 y;
	uint32 *screen;
	
	screen = (uint32*)lfb_screenPtr;

	    for ( x =  (lfb_screenWidth / 4); x < (3 * lfb_screenWidth / 4); x++)
	     for ( y = (lfb_screenHeight / 4); y < (3 * lfb_screenHeight / 4) ; y++)
			screen[ y * 800 + x ]  =  ( y * x ) / ( x + y );

	    
	while (1==1) {}
}


void set_lfb( uint32 width,
              uint32 height,
              uint32 depth,
              uint32 type,
              void * ptr )
{
	lfb_screenWidth  = width;	
	lfb_screenHeight = height;	
	lfb_screenDepth  = depth;	
	lfb_screenType   = type;
	lfb_screenPtr    = ptr;	
}

int get_lfb( uint32 *width,
             uint32 *height,
             uint32 *depth,
             uint32 *type,
             void **ptr )
{
	*width  = lfb_screenWidth;
	*height = lfb_screenHeight;
	*depth  = lfb_screenDepth;
	*type   = lfb_screenType;
	*ptr    = lfb_screenPtr;

	if ( lfb_screenPtr == 0 ) return -1;
	
	return 0;
}


uint32 provide_lfb(  struct process *proc  )
{
	uint32 area;
	uint32 pages;
	uint32 screen;

	if ( using_lfb() != 1 )  return 0;
	if ( proc->lfb != NULL ) return (uint32)proc->lfb;
	

	screen = ((uint32)lfb_screenPtr) & ~0xFFF;
	
	pages = (lfb_screenHeight * lfb_screenWidth * lfb_screenDepth/8) / 4096 + 1;

    area = page_provide( proc->map,
			 	     	 screen,
						 app_BASE,
						 sK_BASE,
						 pages,
						 USER_PAGE | READ_WRITE );

	proc->lfb = (void*)(area + (((uint32)lfb_screenPtr) & 0xFFF));

	return (uint32)(proc->lfb);
}

uint32 release_lfb(  struct process *proc  )
{
	uint32 pages;
	uint32 screen;

	if ( using_lfb() != 1 ) return 0;

	screen = ((uint32)proc->lfb) & ~0xFFF;
	
	pages = (lfb_screenHeight * lfb_screenWidth * lfb_screenDepth/8) / 4096 + 1;
	page_release( proc->map, screen, pages );
	
	proc->lfb = NULL;
	return 0;
}





