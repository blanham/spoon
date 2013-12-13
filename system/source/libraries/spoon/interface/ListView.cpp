#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/ListView.h>
#include <spoon/interface/ScrollBar.h>
#include <spoon/interface/Scrollable.h>
#include <spoon/interface/Window.h>

ListView::ListView( Rect frame, Pulse *pulse, uint32 follows )
: View( frame, follows )
{
	SetBackgroundColour( 0xFFFFFF );
	m_selected = NULL;
	m_msg = NULL;
	m_pulse = pulse;
}

ListView::ListView( Rect frame, Message *msg, uint32 follows )
: View( frame, follows )
{
	SetBackgroundColour( 0xFFFFFF );
	m_selected = NULL;
	m_pulse = NULL;
	m_msg = msg;
}


ListView::~ListView()
{
	if ( m_msg != NULL ) delete m_msg;
	if ( m_pulse != NULL ) delete m_pulse;
}



void ListView::AddItem( ListItem *item )
{
  m_items.add( item );
}


ListItem* ListView::ItemAt( int i )
{
	return (ListItem*)m_items.get( i );
}


ListItem* ListView::RemoveItem( int i )
{
   if ( i < 0 ) return NULL;
   if ( i >= CountItems() ) return NULL;
		
   ListItem* ans = (ListItem*)m_items.get( i );
   m_items.remove( ans );

   if ( ans == m_selected ) m_selected = NULL;
   
   return ans;
}


bool ListView::RemoveItem( ListItem* item )
{ 
    bool ans = m_items.remove( item );
    if ( item == m_selected ) m_selected = NULL;
    return ans;
}


int ListView::CountItems()
{
	return m_items.count();
}


void ListView::DoubleClicked( ListItem *item )
{
	if ( GetWindow() == NULL ) return;

	if ( m_pulse != NULL ) GetWindow()->PostPulse( m_pulse );
	if ( m_msg != NULL ) GetWindow()->PostMessage( m_msg );
}



void ListView::MouseDown( int x, int y, unsigned int buttons )
{

	int index = 0;
	int tmp_y = 1;
		
   while ( index < CountItems() )
   {
	 ListItem *item = ItemAt( index );
	 if ( item == NULL ) return;
	 
	 tmp_y += item->Height();
	 
	 if ( tmp_y > y )
	 {
		Select( index );
		Draw( Bounds() );
		Sync();
		return;
	 }
			 
	 index += 1;
   }
   
   Select(-1);
   
}

// ........

void ListView::Empty()
{
	while ( CountItems() > 0 )
	{
		ListItem *item = RemoveItem( 0 );
		delete item;
	}
	
}

// ------------------------------------------


void ListView::Draw( Rect frame )
{
	DrawRect( Bounds(), 0x444444 );

	Rect place = Rect( 1, 1, Bounds().right - 1, 1 );
   
	for ( int i = 0; i < CountItems(); i++ )
	{
		ListItem *item = ItemAt( i );

			place.bottom = place.top + item->Height();
	
		item->DrawItem( this, place );

		place.top = place.bottom;
	
		if (  place.top >= (Bounds().bottom - 1) ) break;
	}

   if ( place.top < (Bounds().bottom - 1) )
    {
      place.bottom = Bounds().bottom - 1;
      FillRect( place, 0x888888 );
    }
}


bool ListView::Select( int i )
{
	ListItem *old = m_selected;

	if ( m_selected != NULL ) m_selected->Select( false );
	m_selected = ItemAt( i );

	if ( m_selected != NULL ) 
	{
		m_selected->Select( true );
		if ( old == m_selected )
		{
			DoubleClicked( m_selected );
		}

		return true;
	}

	
	return false;
}

ListItem* ListView::Selected()
{
	return m_selected;
}





