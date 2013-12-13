#include "spoon.h"
#include <spoon/interface/Window.h>
#include <spoon/interface/View.h>


static Window* win = NULL;
static View *view = NULL;


void spoon_putpixel( int x, int y, uint32 col )
{
	view->PutPixel( x,y, col );
}

void spoon_sync()
{
	win->Sync();
}

// -----------------------------------

void init_spoon( int width, int height )
{
  win  = new Window( Rect( 5,5, 5 + width, 5 + height ), 
				  	 "mpeg2decode",
				     WINDOW_HAS_BORDER );
  view = new View( Rect(0,0,width,height) );
  win->AddChild( view );
  win->Show();
}

void clean_spoon()
{
  if ( win != NULL )
  {
      win->Hide();
	  win->Quit();
  }
}


