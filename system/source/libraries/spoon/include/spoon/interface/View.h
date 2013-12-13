#ifndef _SPOON_INTERFACE_VIEW_H
#define _SPOON_INTERFACE_VIEW_H


#include <types.h>

#include <spoon/interface/Drawing.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>
#include <spoon/collections/List.h>
#include <spoon/base/Lockable.h>

class Window;
class PopupMenu;

#define 	FOLLOW_LEFT			0x1
#define		FOLLOW_RIGHT		0x2
#define		FOLLOW_TOP			0x4
#define		FOLLOW_BOTTOM		0x8

#define		FOLLOW_LEFT_RIGHT	( FOLLOW_LEFT | FOLLOW_RIGHT )
#define		FOLLOW_TOP_BOTTOM	( FOLLOW_TOP  | FOLLOW_BOTTOM )

#define		FOLLOW_ALL_SIDES	( FOLLOW_LEFT | FOLLOW_RIGHT | FOLLOW_TOP | FOLLOW_BOTTOM )


#define 	PULSE_NEEDED		1
#define 	HAS_POPUPMENU		8


/** \ingroup interface 
 *
 */
class View : public Drawing, public Lockable
{
   friend class Window;

   public:
     View( Rect frame, uint32 follows = FOLLOW_ALL_SIDES,
					   unsigned int flags = 0);
     virtual ~View();


     Rect Frame();
     Rect Bounds(); 


	
	void ResizeTo( int width, int height );
	void ResizeBy( int dw, int dh );
	void MoveTo( int x, int y );
	void MoveBy( int x, int y );

	Rect ConvertToWindow( Rect box );
	Rect ConvertFromWindow( Rect box );

	Point ConvertToWindow( Point p );
	Point ConvertFromWindow( Point p );


	bool  AddChild( View *view );
	View *ChildAt( int i );
	View *RemoveChild( int i );
	bool RemoveChild( View *view );
	int CountChildren();
	

   public:
    virtual void AttachedToWindow();
	virtual void DetachedFromWindow();

	virtual void Activated( bool active = true );

	virtual void MouseDown( int x, int y, unsigned int buttons ) {}
	virtual void MouseUp( int x, int y, unsigned int buttons ) {}
	virtual void MouseMoved( int x, int y, unsigned int buttons ) {}

	virtual void KeyDown( uint32 code, uint32 ascii, uint32 modifiers ) {}
	virtual void KeyUp( uint32 code, uint32 ascii, uint32 modifiers ) {}

	virtual PopupMenu* Popup();
	void InvalidatePopup();

   private:
	void MouseEvent( int what, int x, int y, unsigned int buttons );
	void KeyEvent( uint32 what, uint32 code, uint32 ascii, uint32 modifiers );


   

   public:
	virtual void Pulsed();
   	virtual void Draw( Rect rect );
	void DrawChildren( Rect rect );
    void Sync( int left = 0, int top = 0, int right = -1, int bottom = -1 );

	void SetBackgroundColour( uint32 col );
	uint32 BackgroundColour();
	

	bool Active();
	void SetActive( bool active = true );

	unsigned int getFlags();
	bool hasFlag( unsigned int flag );

   public:
     void PutPixel( int x, int y, uint32 col );
     uint32 GetPixel( int x, int y );


   public:
   	inline Window *GetWindow() 
	  { return _window; }

	inline View *GetParent()
	  { return _parent; }
	


   private:
	Rect    _frame;
    Window *_window;
	View   *_parent;
	List    _views;

	PopupMenu *m_popupMenu;
	unsigned int m_buttons;
	
	unsigned int m_flags;

	uint32  m_bgCol;
	bool _active;
	uint32  _follows;
     
     
};


#endif


