#include <spoon/interface/MenuItem.h>
#include <spoon/interface/PopupMenu.h>

#include "DesktopView.h"

DesktopView::DesktopView( Rect frame )
: View( frame, FOLLOW_LEFT | FOLLOW_RIGHT, HAS_POPUPMENU )
{
	SetBackgroundColour( 0x304060 );
}

DesktopView::~DesktopView()
{
}




void DesktopView::Draw( Rect frame )
{
	View::Draw( frame );
}


PopupMenu *DesktopView::Popup()
{
	if ( GetWindow() == NULL ) return NULL;

	PopupMenu *menu = new PopupMenu( GetWindow() ); 

	menu->AddChild( new MenuItem( "one" ) );
	menu->AddChild( new MenuItem( "two" ) );
	menu->AddChild( new MenuItem( "three" ) );
	menu->AddChild( new MenuItem( "four" ) );
	menu->AddChild( new MenuItem( "five" ) );

	return menu;
}


