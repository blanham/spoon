#ifndef _GUI_SERVER_WINDOW_H
#define _GUI_SERVER_WINDOW_H


#include <types.h>
#include <spoon/interface/Rect.h>


namespace GUI
{

class Desktop;

class Window
{

	public:
		~Window();


		static Window *create(  Rect frame,
							 	char *title,
							 	int pid,
							 	int port,
							 	bool visible,
								unsigned int flags
					   			);
		
		int wid();
		int sid();
		int pid();
		int port();
		unsigned int flags();
		char *title();

		int MouseEvent( uint32 what, int x, int y, unsigned int buttons );
     	int KeyPressed( uint32 what, uint32 code, uint32 ascii, uint32 modifiers );
		int signalFocus( bool focus );


		bool isTransparent();
		bool isVisible();
		bool setVisible( bool vis );


		Rect& Frame();

		bool moveTo( Rect &newFrame );
		bool resizeTo( int width, int height );
		int close();

		uint32* buffer();

		
	private:
		Window();

		Rect m_frame;
		char *m_title;
		int m_pid;
		int m_port;
		int m_wid;
		int m_sid;
		
		unsigned int m_flags;
		
		uint32 *m_buffer;

		bool m_visible;
		



};



}

#endif

