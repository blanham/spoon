#include <spoon/interface/Button.h>
#include <spoon/interface/MenuItem.h>
#include <spoon/interface/PopupMenu.h>

#include "DesktopBar.h"
#include "ClockView.h"
#include "MemView.h"
#include "SystemButton.h"

#include "DotView.h"

DesktopBar::DesktopBar( Rect frame )
: View( frame, FOLLOW_LEFT | FOLLOW_RIGHT )
{
	Rect fr = Rect(2,2,58, Bounds().Height() - 2 );
	
	Button *button = new SystemButton( "System",
										 new Pulse( 0x666 ),
										 fr );

	AddChild( button );

	// ------

	fr.Set( Bounds().Width() - 122, 2, 
			Bounds().Width() - 2, Bounds().Height() - 2 ); 
	ClockView *clockView = new ClockView( fr );
	AddChild( clockView );

	fr.Set( 60, 2, 66, Bounds().Height() - 2 );
	DotView *dotView = new DotView( fr );
	AddChild( dotView );
	
	fr.Set( Bounds().Width() - 130, 2, 
			Bounds().Width() - 124, Bounds().Height() - 2 );
	dotView = new DotView( fr );
	AddChild( dotView );

	fr.Set( Bounds().Width() - 232, 2, 
			Bounds().Width() - 132, Bounds().Height() - 2 ); 
	MemView *memView = new MemView( fr );
	AddChild( memView );

	fr.Set( Bounds().Width() - 240, 2, 
			Bounds().Width() - 234, Bounds().Height() - 2 );
	dotView = new DotView( fr );
	AddChild( dotView );
}

DesktopBar::~DesktopBar()
{
}




void DesktopBar::Draw( Rect frame )
{
	for ( int i = 0; i < Bounds().Height(); i++ )
	{
		uint32 col = 0xC0C0C0;
		if ( ((Frame().top + i) % 3) == 2 ) col = 0xB8B8C8;

		int width = Bounds().Width() - 1;

		int off = 0;

		if ( i == (Bounds().Height() - 4) )  off = 1;
		if ( i == (Bounds().Height() - 3) )  off = 1;
		if ( i == (Bounds().Height() - 2) )  off = 2;
		if ( i == (Bounds().Height() - 1) )  off = 4;
	
		DrawLine( 0, i, width - off, i, col );
	}
}




