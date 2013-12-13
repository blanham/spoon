#ifndef _GUI_SERVER_DESKTOP_H
#define _GUI_SERVER_DESKTOP_H


#include <spoon/collections/List.h>
#include <spoon/ipc/Looper.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Drawing.h>

#include <Window.h>

namespace GUI
{


#define ACTION_PASSTHRU			0
#define ACTION_PASSTHRU_TRANS	1
#define ACTION_DESKTOP			2
#define ACTION_MOVE_START		3
#define ACTION_MOVE_FINISH		4
#define ACTION_CLOSE			99
		
		
		
class Desktop: public List, public Looper, public Drawing
{
	public:
		Desktop();
		~Desktop();

		Rect Bounds();
		
		bool setDesktopWindow( Window *win );
		
		bool drawDesktop();
		bool drawFrame( Window *win );
		void adjustFrameForBorder( Window *win, Rect &frame );

		bool actionAt( uint32 what,
					   int x, int y, unsigned int buttons, 
					   Window **window, int *action );
		
		Window *windowBy( int pid, int wid );

		int syncWindow( Window *win,
						int left, int top,
						int right, int bottom );
		int subSync( Window *win,
						int left, int top,
						int right, int bottom );
	
		
		int showWindow( int pid, int id, bool visible ); 

		bool focus( Window *win );
		bool isFocus( Window *win );

     	void KeyPressed( uint32 what, uint32 code, uint32 ascii, uint32 modifiers );
		void MouseEvent( uint32 what, int x, int y, unsigned int buttons );


		void hideCursor( int x, int y, unsigned int buttons );
		void showCursor( int x, int y, unsigned int buttons );

		
		int deregisterWindow( int pid, int wid );
		int resizeWindow( Message *msg, int pid, int wid, Rect *frame );
		int moveWindow( Message *msg, int pid, int wid, int x, int y );
		
		void MessageReceived( Message *msg );
		void PulseReceived( Pulse *pulse );


    	void PutPixel( int x, int y, uint32 col );
    	uint32 GetPixel( int x, int y );
		bool initScreen();
	private:
		bool m_buffering;
		uint32 *m_double_buffer;
		uint32 *m_lfb;
		int m_width;
		int m_height;
		int m_depth;

		uint32 drawing_lock;

	   int prev_x;
	   int prev_y;
	   unsigned int prev_buttons;
	   uint32  saved[8][8];
 
	   unsigned int modifiers;

		Rect *movingRect;
		Window *movingWindow;

		Window *desktopWindow;
};



}

#endif

