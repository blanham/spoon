#include <types.h>
#include <stdlib.h>

#include <spoon/collections/List.h>

#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>
#include <spoon/interface/Window.h>
#include <spoon/interface/View.h>
#include <spoon/interface/PopupMenu.h>

#define DEFAULT_VIEW_COLOR	0xd4d0c8
	

View::View( Rect frame, uint32 follows, unsigned int flags )
 : _frame( frame ), 
   _window(NULL),
   _parent(NULL),
   m_bgCol( DEFAULT_VIEW_COLOR ), 
   _active(false),
   _follows(follows)
{
	m_popupMenu = NULL;
	m_flags 	= flags;
	m_buttons	= 0;
}


View::~View()
{
   while ( CountChildren() > 0 )
   {
      View *sam = RemoveChild(0);
      delete sam;
   }

   if ( m_popupMenu != NULL ) 
   {
	   m_popupMenu->Quit();
   }
}


// ******************************************************

Rect View::Frame() 
{ 
	return _frame; 
}

Rect View::Bounds() 
{ 
	return Rect(0,0, _frame.Width() -1, _frame.Height() - 1 ); 
}



// ******************************************************


void View::ResizeTo( int width, int height )
{
    _frame.right  = _frame.left + width - 1;
    _frame.bottom = _frame.top + height - 1;
}


void View::ResizeBy( int dw, int dh )
{
   ResizeTo( _frame.Width() + dw, _frame.Height() + dh );
}


void View::MoveTo( int x, int y )
{
   int width  = _frame.Width();
   int height = _frame.Height();

   _frame.left   = x;
   _frame.right  = x + width - 1;
   _frame.top    = y;
   _frame.bottom = y + height - 1;
}


void View::MoveBy( int x, int y )
{
  MoveTo( _frame.left + x, _frame.top + y );
}


// ******************************************************

Rect View::ConvertToWindow( Rect box )
{
   Rect sam = box;
 
   View *parent = this;	// I am my own parent.

   while ( parent != NULL )
   {
      sam.left   += parent->Frame().left; 
      sam.right  += parent->Frame().left; 
      sam.bottom += parent->Frame().top; 
      sam.top    += parent->Frame().top; 

      parent = parent->GetParent();
   }
 

   return sam;
}


Rect View::ConvertFromWindow( Rect box )
{
   Rect sam = box;
 
   View *parent = this;	// I am my own parent.

   while ( parent != NULL )
   {
      sam.left   -= parent->Frame().left; 
      sam.right  -= parent->Frame().left; 
      sam.bottom -= parent->Frame().top; 
      sam.top    -= parent->Frame().top; 

      parent = parent->GetParent();
   }
 

   return sam;
}



Point View::ConvertToWindow( Point p )
{
   Point sam = p;
 
   View *parent = this;	// I am my own parent.

   while ( parent != NULL )
   {
      sam.x   += parent->Frame().left; 
      sam.y   += parent->Frame().top; 

      parent = parent->GetParent();
   }

   return sam;
}


Point View::ConvertFromWindow( Point p )
{
   Point sam = p;
 
   View *parent = this;	// I am my own parent.

   while ( parent != NULL )
   {
      sam.x   -= parent->Frame().left; 
      sam.y   -= parent->Frame().top; 

      parent = parent->GetParent();
   }

   return sam;
}

// ******************************************************



bool View::AddChild( View *view )
{
	bool ans = _views.add( view );
	if ( ans )
	{
     view->_parent = this;
     view->_window = GetWindow();
     if ( GetWindow() != NULL ) view->AttachedToWindow();
	}
	return ans;
}


View *View::ChildAt( int i )
{
	View *ans = (View*)_views.get(i);
	return ans;
}


View *View::RemoveChild( int i )
{
	View *target = (View*)ChildAt( i );
	RemoveChild( target );
	return target;
}


bool View::RemoveChild( View *view )
{
	bool ans =  _views.remove( view );

	if ( ans )
     {
	   if ( (view->getFlags() & PULSE_NEEDED) != 0 )
		   if ( GetWindow() != NULL ) GetWindow()->RemovePulseChild( view );

			 
       view->_parent = NULL;
	   if (view->_window != NULL )
	   {
		 view->_window = NULL;
		 view->DetachedFromWindow();
	   }

     }
    return ans;
}

int View::CountChildren()
{
	int ans = _views.count();
	return ans;
}

// ******************************************************


void View::AttachedToWindow()
{
  lock();
  
  	// First, add to the Window's pulse list if this view needs it.
	  if ( (getFlags() & PULSE_NEEDED) != 0 )
		  if ( GetWindow() != NULL ) GetWindow()->AddPulseChild( this );


  // ...
  for ( int i = 0; i < CountChildren(); i++ )
   {
     View *view = ChildAt(i);
           view->_window = GetWindow();
	       view->AttachedToWindow();
   }

  unlock();
}

void View::DetachedFromWindow()
{
  lock();
  for ( int i = 0; i < CountChildren(); i++ )
   {
     View *view = ChildAt(i);

	   if ( (view->getFlags() & PULSE_NEEDED) != 0 )
		   if ( view->_window != NULL ) view->_window->RemovePulseChild( view );
	 
           view->_window = NULL;
	       view->DetachedFromWindow();
   }
  unlock();
}

void View::SetActive( bool active )
{ 
  _active = active; 
  Activated( active );
}

void View::Activated( bool active ) 
{ 
}

bool View::Active()
{ 
	return _active; 
}


unsigned int View::getFlags()
{
	return m_flags;
}

bool View::hasFlag( unsigned int flag )
{
	return ( (getFlags() & flag) == flag );
}

// ******************************************************
	
void View::Pulsed()
{
	// chill... relax.... you know.
}

void View::Draw( Rect rect )
{
   Rect area = Bounds();
   FillRect( area, BackgroundColour() );
}

void View::DrawChildren( Rect rect )
{
   if ( GetWindow() == NULL ) return;

   lock();
   for ( int i = 0; i < CountChildren(); i++ ) 
   {
     View *child = ChildAt(i);
           child->Draw( rect );
		   child->DrawChildren( rect );
   }
   unlock();
}


void View::Sync( int left, int top, int right, int bottom )
{
	if ( GetWindow() == NULL ) return;

	Rect box;
	if ( (left == 0) && (top == 0) && (right == -1) && (bottom == -1) )
	{ 
		box = ConvertToWindow( Bounds() );
	}
	else
	{
		box = ConvertToWindow( Rect( left, top, right, bottom ) );
	}

	if ( box.IsValid() == false ) return;

	GetWindow()->Sync( box.left, box.top, box.right, box.bottom );
}

// ******************************************************

void View::SetBackgroundColour( uint32 col )
{ 
	m_bgCol = col; 
}

uint32 View::BackgroundColour()
{ 
	return m_bgCol; 
}

// **********************************************************

PopupMenu* View::Popup()
{
	return NULL;
}

void View::InvalidatePopup()
{
	if ( m_popupMenu == NULL ) return;
	m_popupMenu->Quit();
}

// **********************************************************


void View::KeyEvent( uint32 what, 
		     uint32 code,
		     uint32 ascii,
		     uint32 modifiers )
{
   switch (what)
   {
     case KEY_DOWN:
     	KeyDown( code, ascii, modifiers );
     	break;

     case KEY_UP:
     	KeyUp( code, ascii, modifiers );
     	break;
   }
}


void View::MouseEvent( int what, int x, int y, unsigned int buttons )
{

			// Backwards, as added.
  lock();
  for ( int i = CountChildren() - 1; i >= 0; i-- )  
  {
     View *view = ChildAt(i);

        if ( view->Frame().Contains( x,y ) )
         {
		   int nx = x - view->Frame().left;
		   int ny = y - view->Frame().top;

		   if ( GetWindow() != NULL ) GetWindow()->SetActiveView( view );
		   
		   view->MouseEvent( what, nx, ny, buttons );
		   unlock();
		   return;
	  	 }
  }
  unlock();
 

	// Otherwise...
 
  	   switch( what )
	   {
	     case MOUSE_DOWN:
            MouseDown( x, y, buttons );
		    break;
	     case MOUSE_UP:
			if ( hasFlag( HAS_POPUPMENU ) == false )
			{
			   MouseUp( x, y, buttons );
			   break;
			}
			// Only the right mouse button.
			
			if ( m_buttons != RIGHT_MOUSE_BUTTON )
			{
				MouseUp( x, y, buttons );
				break;
			}

			if ( m_popupMenu == NULL ) m_popupMenu = Popup();
			if ( m_popupMenu == NULL ) return;
			
			m_popupMenu->MoveTo(  x - m_popupMenu->Frame().Width() / 2,
								  y - m_popupMenu->Frame().Height() / 2 );
			
			m_popupMenu->Show();
		    break;

	     case MOUSE_MOVED:
            MouseMoved( x, y, buttons );
		    break;
	   }

	m_buttons = buttons;
}
// ----------------------------------------------------

void View::PutPixel( int x, int y, uint32 col )
{
    if ( GetWindow() == NULL ) return;

    if ( (y < 0) || (y > Bounds().bottom) ) return;
    if ( (x < 0) || (x > Bounds().right)  ) return;
    
    Point a(x,y);
    a = ConvertToWindow( a );
    GetWindow()->PutPixel( a.x, a.y, col );
}

uint32 View::GetPixel( int x, int y )
{
   if ( GetWindow() == NULL ) return 0;

    if ( (y < 0) || (y > Bounds().bottom) ) return 0;
    if ( (x < 0) || (x > Bounds().right)  ) return 0;
    
   Point a(x,y);
   a = ConvertToWindow( a );
   return GetWindow()->GetPixel( a.x, a.y );
}




