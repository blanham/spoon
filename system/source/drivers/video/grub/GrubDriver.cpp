#include <types.h>
#include <stdio.h>
#include <spoon/video/VideoDefs.h>
#include <spoon/video/VideoDriver.h>

#include "GrubDriver.h"


GrubDriver::GrubDriver()
: VideoDriver("grub", "Supporting GRUB VESA modes set on boot.", false)
{
	m_lfb = NULL;
}

GrubDriver::~GrubDriver()
{
}
	
int   GrubDriver::setMode( int width, int height, int depth )
{
	if ( (unsigned)width != m_width ) return -1;
	if ( (unsigned)height != m_height ) return -1;

	Indicate(1);

	return 0;
}

void* GrubDriver::getLFB( int *width, int *height, int *depth )
{
	*width = m_width;
	*height = m_height;
	*depth = m_depth;
	
	Indicate(2);

	return m_lfb;
}

void GrubDriver::Indicate(int num)
{
	return;
	for ( unsigned int i = 0; i < m_width; i++ )
	{
		((uint32*)m_lfb)[i + ( num * 10 ) * m_width] = 0xFF0000;
	}
}


int	GrubDriver::init()
{
	void *ptr;
	uint32 type;

	printf("%s\n","getting lfb information");
	
	if ( smk_get_lfb_info( &m_width, &m_height, &m_depth, &type, &ptr ) != 0 )
		return -1;

	int pages = (m_width * m_height * sizeof( unsigned int ) ) / 4096 + 1;
//	      ptr = ptr - (ptr & 0xFFF);
	
	printf("%s\n","memory mapping LFB.");
		  
	m_lfb = smk_mem_map( ptr, pages );
	if ( m_lfb == NULL ) return -1;


	printf("%s\n","adding mode.");
	
	addMode( m_width, m_height, RGB32 );
	
	
	uint32 *lfb = (uint32*)m_lfb;
	for ( unsigned int i = 0; i < m_width; i++ )
	 for ( unsigned int j = 0; j < m_height; j++ )
	 {
		//lfb[ j * m_width + i ] = 0xABCDEF;
		lfb[ j * m_width + i ] = 0x402020;
	 }
	
	return 0;
}

int	GrubDriver::shutdown()
{
	if ( m_lfb != NULL ) smk_mem_free( m_lfb );
	m_lfb = NULL;
	return 0;
}
 
int	GrubDriver::sync( int x, int y, int width, int height )
{
	Indicate(3);
	return 0;
}


