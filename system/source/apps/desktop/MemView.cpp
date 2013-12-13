#include <types.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Bitmap.h>

#include "MemView.h"

MemView::MemView( Rect frame )
: View( frame, FOLLOW_ALL_SIDES, PULSE_NEEDED )
{
	m_background = NULL;
}

MemView::~MemView()
{
	if ( m_background != NULL ) 
			delete m_background;
}

void MemView::Draw( Rect frame )
{
	static int old_free = 0; 
	if ( old_free == smk_info_mem_free() ) return; //Only draw if needed.

	old_free = smk_info_mem_free();
	
	char message[256];
	  sprintf(message,"%i/%i",
					  (smk_info_mem_total() - smk_info_mem_free()),
					  smk_info_mem_total() / 1024 );
	
		if ( m_background == NULL )
		{
			m_background = new Bitmap( Bounds().Width(), Bounds().Height() );
			
			for ( int i = 0; i < Bounds().Width(); i++ )
			 for ( int j = 0; j < Bounds().Height(); j++ )
				m_background->PutPixel( i, j, GetPixel(i,j) );
		}
		
		DrawBitmap( 0,0, m_background );
	
	  
	  int x = Bounds().Width() / 2 - (strlen(message) * 8) / 2;
	  int y = Bounds().Height() / 2 - 8;

	DrawString( x, y, message , 0 );
	Sync();
}

void MemView::Pulsed()
{
	Draw( Bounds() );
}

