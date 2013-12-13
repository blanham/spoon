#include <types.h>
#include <stdio.h>
#include <spoon/video/VideoDefs.h>
#include <spoon/video/VideoDriver.h>

#include "VMWareDriver.h"

#include "svga_reg.h"

VMWareDriver::VMWareDriver( unsigned int index, unsigned int value )
: VideoDriver("vmware", "A VMWare compatible video driver", true)
{
	index_reg = index;
	value_reg = value;

	printf("%s: index = %x, value = %x\n",
				"vmware", 
				index_reg,
				value_reg );
}

VMWareDriver::~VMWareDriver()
{
}
	
int   VMWareDriver::setMode( int width, int height, int depth )
{
	printf("%s: %i %i %i\n","Request to set mode",
							width,
							height,
							depth );
	
/*
    SetMode
	Set SVGA_REG_WIDTH, SVGA_REG_HEIGHT, SVGA_REG_BITS_PER_PIXEL
	Read SVGA_REG_FB_OFFSET
	(SVGA_REG_FB_OFFSET is the offset from SVGA_REG_FB_START of the
	 visible portion of the frame buffer)
	Read SVGA_REG_BYTES_PER_LINE, SVGA_REG_DEPTH, SVGA_REG_PSEUDOCOLOR,
	SVGA_REG_RED_MASK, SVGA_REG_GREEN_MASK, SVGA_REG_BLUE_MASK
*/


	OUT( SVGA_REG_WIDTH, width );
	OUT( SVGA_REG_HEIGHT, height );
	OUT( SVGA_REG_BITS_PER_PIXEL, depth );

	IN( SVGA_REG_FB_OFFSET );
	IN( SVGA_REG_BYTES_PER_LINE );
	IN( SVGA_REG_DEPTH );
	IN( SVGA_REG_PSEUDOCOLOR );
	IN( SVGA_REG_RED_MASK );
	IN( SVGA_REG_GREEN_MASK );
	IN( SVGA_REG_BLUE_MASK );

	OUT( SVGA_REG_ENABLE, 1 );
	
	return 0;
}

void* VMWareDriver::getLFB( int *width, int *height, int *depth )
{
	printf("%s\n","request to get LFB" );
	return NULL;
}

int	VMWareDriver::init()
{
	OUT( SVGA_REG_ID, SVGA_ID_2 );
	if ( IN( SVGA_REG_ID ) ==  SVGA_ID_2 )
	{
		printf("%s\n","vmware: SVGA_ID_2 supported" );
	}
	else
	{
		OUT( SVGA_REG_ID, SVGA_ID_1 );
		if ( IN( SVGA_REG_ID ) ==  SVGA_ID_1 ) 
		{
			printf("%s\n","vmware: SVGA_ID_1 supported" );
		}
		else
		{
			OUT( SVGA_REG_ID, SVGA_ID_0 );
			if ( IN( SVGA_REG_ID ) ==  SVGA_ID_0 ) 
			{
				printf("%s\n","vmware: SVGA_ID_0 supported" );
			}
		}
	}
			 
	// -----------------
		
		
	addMode( 640, 480, RGB32 );
	addMode( 800, 600, RGB32 );
	addMode( 1024, 768, RGB32 );
	return 0;
}

int	VMWareDriver::shutdown()
{
	return -1;
}
 

int	VMWareDriver::sync( int x, int y, int width, int height )
{
	return 0;
}



// --------------------------


void VMWareDriver::OUT( int port, unsigned int value )
{
	outw( index_reg, port );
	outw( value_reg, value );
}

int VMWareDriver::IN( int port )
{
	outw( index_reg, port );
	return inw( value_reg );
}

