#ifndef _SPOON_INTERFACE_LISTITEM_H
#define _SPOON_INTERFACE_LISTITEM_H

#include <types.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Rect.h>

/** \ingroup interface 
 *
 */
class ListItem
{
   public:
    ListItem();
    virtual ~ListItem(); 

	bool isEnabled();
	bool Enable( bool enable = true );
	
	bool isSelected();
	bool Select( bool select = true );


	virtual void DrawItem( View *owner, Rect frame ) = 0;
	virtual	int Height() = 0;
	virtual	int Width() = 0;

   private:
     bool m_selected;
     bool m_enabled;

};

#endif


