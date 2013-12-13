#include <types.h>
#include <string.h>
#include <spoon/support/String.h>
#include <spoon/interface/StringItem.h>


StringItem::StringItem( const char *label )
: ListItem()
{
	m_label = strdup( label );
}


StringItem::~StringItem()
{
	if ( m_label != NULL ) free( m_label );
}



void StringItem::DrawItem( View *owner, Rect frame )
{
   if ( isSelected() )
    {
    	owner->FillRect( frame, 0x404050 );
		owner->DrawRect( frame, 0 );
    }
   else
    {
    	owner->FillRect( frame, 0xAAAAAA );
    }


	if ( Label() == NULL ) return;


	if ( Width() <= frame.Width() ) 
	 {
		   owner->DrawString( frame.left + 2,
		   		      frame.top + 2,
				      (char*)Label(),
				      0x0 );
	   return;
	 }

	int playfield = frame.Width() - 2 - 2;
	// we have playfield area left to show our text.. 

	playfield = playfield / 8;

	for ( int i = 0; i < playfield; i++ )
	 {
	   char letter[2];
	   letter[0] = m_label[i];
	   letter[1] = 0;
	   owner->DrawString( frame.left + 2 + i * 8,  
	   		      frame.top + 2,
			      letter,
			      0x0 );
	 }
}


int StringItem::Height()
{
	return ( 2 + 16 + 2 );
}


int StringItem::Width()
{
	if ( m_label == NULL ) return 4;
	return ( 2 + strlen( m_label ) * 8 + 2 );
}
	
	
const char *StringItem::Label()
{
	return m_label;
}



