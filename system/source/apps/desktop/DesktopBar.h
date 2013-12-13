#ifndef _DESKTOP_DESKTOPBAR_H
#define _DESKTOP_DESKTOPBAR_H

#include <spoon/interface/Rect.h>
#include <spoon/interface/View.h>
#include <spoon/interface/PopupMenu.h>

class DesktopBar : public View
{
	public:
		DesktopBar( Rect frame );
		~DesktopBar();

		void Draw( Rect frame );

};



#endif

