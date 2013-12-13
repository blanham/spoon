#ifndef _SPOON_INTERFACE_STRINGITEM_H
#define _SPOON_INTERFACE_STRINGITEM_H

#include <spoon/support/String.h>
#include <spoon/interface/View.h>

#include "ListItem.h"

/** \ingroup interface 
 *
 */
class StringItem : public ListItem
{
  public:
    StringItem( const char *label );
	virtual ~StringItem();

	virtual void DrawItem( View *owner, Rect frame );

	virtual	int Height();
	virtual	int Width();

	const char *Label();


  private:
	char *m_label;
};

#endif


