#include <types.h>
#include <stdlib.h>
#include <spoon/interface/Bitmap.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Drawing.h>


// *********************************************************

Bitmap::Bitmap()
{
	m_data = NULL;
	m_width = 0;
	m_height = 0;
	m_bgCol = 0xFFFFFF;
	
	m_rect.Set( 0,0, m_width, m_height );
}

Bitmap::Bitmap( int width, int height, void *data )
{
	m_width = width;
	m_height = height;
	m_data = (uint32*)data;
	m_bgCol = 0xFFFFFF;

	if ( m_width < 0 ) m_width = 0;
	if ( m_height < 0 ) m_height = 0;

	if ( data == NULL )
	{
		m_data = (uint32*)malloc( m_width * m_height * sizeof( uint32 ) );
	}
	
	
	m_rect.Set( 0,0, m_width, m_height );
}

Bitmap::~Bitmap()
{
	if ( m_data != NULL ) free( m_data );
}

// *********************************************************

bool Bitmap::resize( int width, int height, int mode )
{
	uint32 *new_data;

	if ( width < 0 ) return false;
	if ( height < 0 ) return false;

	// Special case of width x height = 0
	if ( (width == 0) || (height == 0) )
	{
		if ( m_data != NULL ) free( m_data );
		m_width = 0;
		m_height = 0;
		m_rect.Set( 0,0, m_width, m_height );
		return true;
	}

	// Allocate new buffer.
	new_data = (uint32*)malloc( width * height * sizeof( uint32) );
	if ( new_data == NULL ) return false;
	
	// --- special case of old image = 0x0
	
	if ( m_data == NULL )
	{
		m_width = width;
		m_height = height;
		m_data = new_data;
		for ( int i = 0; i < width * height; i++ )
			m_data[i] = m_bgCol;
		m_rect.Set( 0,0, m_width, m_height );
		return true;
	}
	

	
	// --- do the resize conversion ----

	bool success = false;

	switch ( mode )
	{
		case BITMAP_CROP:
				success = resize_crop( width, height, new_data );
				break;
		case BITMAP_SCALE:
				success = resize_scale( width, height, new_data );
				break;
		default:
				success = false;
				break;
	}

	// ----------------------

	if ( success == false )
	{
		free( new_data );
		return false;
	}
	
	
	free( m_data );
	
		m_width = width;
		m_height = height;
		m_data = new_data;

		m_rect.Set( 0,0, m_width, m_height );

	return true;
}

const Rect& Bitmap::Frame()
{
	return m_rect;
}

uint32* Bitmap::image_data()
{
	return m_data;
}

// *********************************************************

void Bitmap::PutPixel( int x, int y, uint32 col )
{
	if ( ( x < 0 ) || ( x >= m_width ) ) return;
	if ( ( y < 0 ) || ( y >= m_height ) ) return;

	if ( m_data == NULL ) return;

	m_data[ m_width * y + x ] = col;
}

uint32 Bitmap::GetPixel( int x, int y )
{
	if ( ( x < 0 ) || ( x >= m_width ) ) return m_bgCol;
	if ( ( y < 0 ) || ( y >= m_height ) ) return m_bgCol;
	
	return m_data[ m_width * y + x ];
}
		

// *********************************************************



bool Bitmap::resize_crop( int width, int height, uint32 *new_data )
{
	int max_width  = ( width > m_width ) ? width : m_width;
	int max_height = ( height > m_height ) ? height : m_height;
	
		
	for ( int i = 0; i < max_width; i++ )
	{
		 for ( int j = 0; j < max_height; j++ )
		 {
			uint32 col;

			if ( (i < m_width) && (j < m_height) )
					col = m_data[ m_width * j + i ];
			else
					col = m_bgCol;

			if ( (i < width) && (j < height) )
					new_data[ width * j + i ] = col;
		 }
	}

	return true;
}

bool Bitmap::resize_scale( int width, int height, uint32 *new_data )
{
	// Well, I want to ensure that all pixels in the new image
	// are set. So I will use their width and height values to
	// iterate.
		
	for ( int i = 0; i < width; i++ )
	{
	  for ( int j = 0; j < height; j++ )
	  {
		int x = (i * m_width) / width;
		int y = (j * m_height) / height;

		uint32 col = m_data[ y * m_width + x ]; 

		new_data[ j * width + i ] = col;

	  }
	}

	 
		
	return true;
}



