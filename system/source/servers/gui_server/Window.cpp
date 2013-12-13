#include <kernel.h>
#include <stdlib.h>
#include <string.h>

#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/interface/Window.h>

#include <Window.h>
#include <Desktop.h>

namespace GUI
{

// Class variables.
static int window_id = 0;
		
		
Window* Window::create( Rect frame,
					 	char *title,
					 	int pid,
					 	int port,
					 	bool visible,
			   			unsigned int flags	)
{
	Window *win = new Window();
	if ( win == NULL ) return NULL;

	// Create the local window
	int pages =  (sizeof(uint32) * frame.Height() * frame.Width()) / 4096 + 1;
	
	win->m_sid = smk_create_shmem( "WINDOW", 
									pages, 
									SHMEM_RDWR | SHMEM_PROMISCUOUS );
	if ( win->m_sid < 0 )
	{
		delete win;
		return NULL;
	}

	int tmp_pages;
	unsigned int tmp_flags;
	
	if ( smk_request_shmem( win->m_sid, 
							(void**)&(win->m_buffer),
							&tmp_pages,
							&tmp_flags ) != 0 )
	{
		delete win;
		return NULL;
	}
		
	// Attempt to set the window up correctly with local drawing area and have it shared.
	
		win->m_title = strdup( title );
		win->m_wid = window_id++;
		win->m_pid = pid;
		win->m_port = port;
		win->m_visible = visible;
		win->m_flags = flags;
		win->m_frame = frame;
		
	
	
	return win;
}


/* ------------------------------------- */

Window::Window()
{
	m_buffer = NULL;
	m_wid = -1;
	m_sid = -1;
	m_title = NULL;
}

Window::~Window()
{
	if ( m_title != NULL ) free( m_title );
	if ( m_buffer != NULL ) smk_mem_free( m_buffer );
}


int Window::wid() { return m_wid; }
int Window::sid() { return m_sid; }
int Window::port() { return m_port; }
int Window::pid() { return m_pid; }
unsigned int Window::flags() { return m_flags; }
char* Window::title() { return m_title; }

uint32* Window::buffer() { return m_buffer; }


bool Window::isTransparent()
{
	return ((m_flags & WINDOW_TRANSPARENT) == WINDOW_TRANSPARENT);
}

bool Window::isVisible()
{
	return m_visible;
}

bool Window::setVisible( bool vis )
{
	m_visible = vis;
	return m_visible;
}







Rect& Window::Frame()
{
	return m_frame;
}



bool Window::moveTo( Rect &newFrame )
{
	m_frame = newFrame;
	return true;
}

bool Window::resizeTo( int width, int height )
{
	int pages =  (sizeof(uint32) * width * height) / 4096 + 1;

	// Attempt to share	with the registering application.
	int new_sid = smk_create_shmem( "WINDOW", 
									pages, 
									SHMEM_RDWR | SHMEM_PROMISCUOUS );
	if ( new_sid < 0 ) return false;
	
	// Well, we have set up the new system correctly. We can
	// lose the old one.
	
	int tmp_pages;
	unsigned int tmp_flags;
	if ( smk_request_shmem( new_sid, 
							(void**)&(m_buffer),
							&tmp_pages,
							&tmp_flags ) != 0 ) 
	{
		smk_delete_shmem( new_sid );
		return false;
	}
	
	smk_release_shmem( m_sid );
	
	m_sid 		= new_sid;
	m_frame.right  = m_frame.left + width - 1;
	m_frame.bottom = m_frame.top + height - 1;

	return true;
}


int Window::close()
{
	return Messenger::SendPulse( m_pid, m_port, QUIT_REQUESTED );
}



int Window::MouseEvent( uint32 what, int x, int y, unsigned int buttons )
{
	return Messenger::SendPulse( m_pid, m_port, what, (x-m_frame.left), (y-m_frame.top), buttons );
}


int Window::KeyPressed( uint32 what, uint32 code, uint32 ascii, uint32 modifiers )
{
	return Messenger::SendPulse( m_pid, m_port, what, code, ascii, modifiers );
}

int Window::signalFocus( bool focus )
{
	if ( focus == true ) 
			return Messenger::SendPulse( m_pid, m_port, FOCUS, 1 );

	return Messenger::SendPulse( m_pid, m_port, FOCUS, 0 );
}




}

