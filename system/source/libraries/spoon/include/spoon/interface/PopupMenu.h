#ifndef _SPOON_INTERFACE_POPUPMENU_H
#define _SPOON_INTERFACE_POPUPMENU_H

#include <spoon/interface/Rect.h>
#include <spoon/interface/Window.h>
#include <spoon/ipc/Looper.h>

class MenuItem;


/** \ingroup interface 
 *
 */
class PopupMenu : public Window
{
	public:
		PopupMenu( Looper *looper );
		bool AddChild( View *view );

		void Focus( bool focus );
		void Draw( Rect frame );

		void PulseReceived( Pulse *pulse );
		void MessageReceived( Message *msg );

		void Trigger( MenuItem* item,
					  Pulse *pulse,
					  Message *message );

	private:
     	void MouseEvent( int what, int x, int y, unsigned int buttons );

		MenuItem *m_lastItem;
		Looper *m_looper;

		int m_itemCount;
		
};


#endif

