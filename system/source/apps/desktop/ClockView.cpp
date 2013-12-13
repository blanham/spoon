#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <spoon/interface/View.h>
#include <spoon/interface/Bitmap.h>

#include "ClockView.h"

ClockView::ClockView( Rect frame )
: View( frame, FOLLOW_ALL_SIDES, PULSE_NEEDED )
{
	m_date_format = NULL;
//	setDateFormat(  "%a %d, %B %Y, %H:%M:%S" );
	setDateFormat(  "%d %b %H:%M" );
	m_background = NULL;
}

ClockView::~ClockView()
{
	if ( m_date_format != NULL ) free( m_date_format );
	delete m_background;
}


void ClockView::setDateFormat( const char *format )
{
	char *old = m_date_format;
	m_date_format = strdup( format );
	if ( old != NULL ) free( old );
}


void ClockView::Draw( Rect frame )
{
	int x;
	int y;

	static time_t old = 0;

	// Get the time ...
	struct tm TM;
	time_t t = time(NULL);

	// Only do minutes changes.
	if ( old == (t / 60) ) return;
	old = t / 60;
	
	char buffer[64];

	localtime_r( &t, &TM );
	strftime( buffer, 64, m_date_format, &TM );
   // Have it ...
	
	if ( m_background == NULL )
	{
		m_background = new Bitmap( Bounds().Width(), Bounds().Height() );
		
		for ( int i = 0; i < Bounds().Width(); i++ )
		 for ( int j = 0; j < Bounds().Height(); j++ )
			m_background->PutPixel( i, j, GetPixel(i,j) );
	}
	
	DrawBitmap( 0,0, m_background );
	
	  x = Bounds().Width() / 2 - (strlen(buffer) * 8) / 2;
	  y = Bounds().Height() / 2 - 8;

	  DrawString( x, y, buffer , 0 );

	Sync();
}


void ClockView::Pulsed()
{
	Draw( Bounds() );
}

