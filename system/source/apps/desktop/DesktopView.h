#ifndef _DESKTOP_DESKTOPVIEW_H
#define _DESKTOP_DESKTOPVIEW_H

#include <spoon/interface/View.h>

class PopupMenu;

class DesktopView : public View
{
	public:
		DesktopView( Rect frame );
		~DesktopView();

		void Draw( Rect frame );

		virtual PopupMenu *Popup();
		
};



#endif

