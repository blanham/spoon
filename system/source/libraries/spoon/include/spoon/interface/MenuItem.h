#ifndef _SPOON_INTERFACE_MENUITEM_H
#define _SPOON_INTERFACE_MENUITEM_H

#include <spoon/interface/View.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>

/** \ingroup interface 
 *
 */
class MenuItem : public View
{
	public:
		MenuItem( const char *str, Pulse *pulse = NULL );
		MenuItem( const char *str, Message *message );
		~MenuItem();

		virtual void Draw( Rect frame );
		
		void MouseUp( int x, int y, unsigned int buttons );

		bool IsSelected();
		void Select( bool select );

		int Width();
		int Height();

	private:
		bool m_selected;
		
		char *m_str;
		Pulse *m_pulse;
		Message *m_message;
		
};


#endif

