#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VideoDriver.h"


namespace GUI
{

static int video_id = 0;


VideoDriver* VideoDriver::create( int pid, int port,
								  char *name,
								  char *description,
								  bool sync,
								  List *modes,
								  int *id )
{
	VideoDriver *vd = new VideoDriver();
	if ( vd == NULL ) return NULL;

	  vd->m_id = video_id++;
	  vd->m_pid = pid;
	  vd->m_port = port;
	  vd->m_name = strdup( name );
	  vd->m_description = strdup( description );
	  vd->m_sync = sync;
	  vd->m_supported_modes = *modes;

	  vd->m_sid = -1;
	  vd->m_LFB = NULL;

		
	return vd;
}




int VideoDriver::id()
{
	return m_id;
}


bool VideoDriver::getMode( int num, int *width, int *height, int *mode )
{
	return false;
}

bool VideoDriver::hasMode( int width, int height, int mode )
{
	return false;
}

bool VideoDriver::setMode( int width, int height, int mode )
{
	return false;
}

			
VideoDriver::VideoDriver()
{
	m_id = -1;
	m_pid = -1;
	m_port = -1;
	m_sync = false;
	m_name = NULL;
	m_description = NULL;

	m_sid = -1;
	m_LFB = NULL;
}

VideoDriver::~VideoDriver()
{
	if ( m_name != NULL ) free( m_name );
	if ( m_description != NULL ) free( m_description );
}

}


