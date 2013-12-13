#ifndef _SPOON_INTERFACE_LISTVIEW_H
#define _SPOON_INTERFACE_LISTVIEW_H


#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Rect.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>

#include "ListItem.h"


/** \ingroup interface 
 *
 */
class ListView : public View
{
   public:
     ListView( Rect frame, 
					Pulse *pulse = NULL, 
					uint32 follows = FOLLOW_ALL_SIDES );
     ListView( Rect frame, 
					Message *msg, 
					uint32 follows = FOLLOW_ALL_SIDES );
    ~ListView(); 


	virtual void Draw( Rect frame );

	bool Select( int i );
	ListItem* Selected();

	virtual void DoubleClicked( ListItem *item );

	virtual void MouseDown( int x, int y, unsigned int buttons );

	void Empty();

	void AddItem( ListItem *item );
	ListItem* ItemAt( int i );
	ListItem* RemoveItem( int i );
	bool RemoveItem( ListItem* item );
	int CountItems();

   private:
     List m_items;
	 ListItem *m_selected;
	 
	 Message *m_msg;
	 Pulse *m_pulse;
};

#endif

