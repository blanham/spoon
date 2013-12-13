#ifndef _SPOON_INTERFACE_WINDOW_H
#define _SPOON_INTERFACE_WINDOW_H

#include <spoon/sysmsg.h>
#include <spoon/support/String.h>
#include <spoon/collections/List.h>

#include <spoon/base/Pulser.h>

#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Pulse.h>

#include <spoon/interface/Drawing.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>
#include <spoon/interface/View.h>

#include <spoon/interface/InterfaceDefs.h>


class Application;


#define WINDOW_HAS_TITLE		(1<<0)
#define WINDOW_HAS_BORDER		(1<<1)
#define WINDOW_HAS_NOFRAME		(1<<2)


#define WINDOW_GETS_MOUSEMOVED	(1<<5)

#define WINDOW_TRANSPARENT		(1<<7)
#define WINDOW_DESKTOP			(1<<12)


/** \ingroup interface 
 *
 */
class Window : public Looper, public Drawing
{

  public:
      Window( Rect frame, 
			  const char *title, 
			  unsigned int flags = WINDOW_HAS_TITLE );

      virtual ~Window();
     
     Rect Frame(); 
     Rect Bounds(); 

	 bool ResizeTo( int width, int height );
	 bool MoveTo( int x, int y );
	 
   
     int Show();
     int Hide();
	 bool IsHidden();
     
     void Sync( int left = 0, int top = 0, int right = -1, int bottom = -1 );
     
  public:
     virtual void Focus( bool focus );
     bool IsFocus();

     uint32 BackgroundColour();
     void SetBackgroundColour( uint32 col );

	 unsigned int SetPulseRate( unsigned int milliseconds );
	 virtual void Pulsed();


     virtual void Draw( Rect frame );
     void DrawViews( Rect frame );
     virtual void WindowMoved( Rect frame ) {};

  public:
	virtual bool  AddChild( View *view );
	View *ChildAt( int i );
	View *ChildAt( int x, int y );
	View *RemoveChild( int i );
	bool  RemoveChild( View *view );
	int   CountChildren();


	void QuitRequested();
    void PulseReceived( Pulse *pulse );

	void  SetActiveView( View *view );
	View* ActiveView();


  private:	// internal house keeping.
     int Register();
     int Deregister();

  protected:
     virtual void MouseEvent( int what, int x, int y, unsigned int buttons );
     virtual void KeyEvent( uint32 what, uint32 code, uint32 ascii, uint32 modifiers );



     

//   protected:  
	/// \todo Take out once there are Bitmap classes.
    public:  
 	 friend class View;
	
     void PutPixel( int x, int y, uint32 col );
     uint32 GetPixel( int x, int y );
  
	 bool AddPulseChild( View* view );
	 bool RemovePulseChild( View *view );

     
  private:
	 friend class Application;
	 Application *_application;
	 
	 
     Rect 	   m_frame;
	 Pulser    *m_pulser;
	 
     int m_sid;
     int m_wid;
	 int m_did;
	 uint32 *m_buffer;

	 unsigned int m_pulseRate;
	 
     bool m_focus;
	 bool _hidden;
	 bool _initialShow;

     View *_active_view;
	 

	 uint32 _bgCol;
	 uint32 m_flags;
	 char *m_title;


     List _views;
	 List m_pulseViews;

};


#endif

