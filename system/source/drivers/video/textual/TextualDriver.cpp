#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>

#include <spoon/video/VideoDefs.h>
#include <spoon/video/VideoDriver.h>

#include "TextualDriver.h"


// Ascii characters arranged according to the number of pixels used to create them and the area of character. Least intensive to most intensive
char real_intensity[256] = { 0, 32, 255, 250, 46, 249, 45, 
		96, 196, 95, 39, 44, 58, 126, 59, 7, 61, 94, 28, 248, 
		34, 43, 246, 47, 169, 170, 26, 27, 205, 124, 60, 62, 
		253, 167, 192 , 217, 247, 175, 174, 9, 41, 40, 29, 
		243, 242, 105, 218, 191, 240, 139, 92, 190, 212, 37, 
		231, 241, 193, 166, 108, 161, 141, 125, 123, 252, 173, 
		73, 91, 33, 93, 42, 99, 114, 194, 115, 4, 238, 168, 
		63, 120, 118, 24, 25, 207, 116, 127, 49, 140, 55, 
		22, 97, 122, 135, 213, 184, 102, 110, 229, 101, 
		117, 226, 236, 111, 18, 67, 74, 76, 30, 31, 245, 
		230, 106, 179, 224, 228, 50, 51, 132, 89, 209, 
		176, 200, 129, 137, 211, 70, 52, 53, 148, 15, 
		133, 160, 13, 84, 251, 155, 254, 145, 188, 227, 
		189, 235, 202, 244, 80, 83, 57, 163, 180, 138, 
		156, 195, 130, 151, 232, 119, 19, 12, 23, 208, 
		100, 98, 107, 54, 5, 149, 131, 162, 1, 88, 159, 
		233, 239, 11, 71, 104, 136, 16, 17, 128, 214, 113, 
		112, 237, 147, 198, 134, 181, 86, 157, 90, 197, 183, 
		65, 164, 121, 150, 142, 144, 225, 69, 103, 210, 171, 
		3, 38, 85, 234, 154, 172, 35, 68, 79, 75, 6, 153, 152, 
		109, 56, 72, 36, 82, 143, 21, 216, 201, 146, 66, 64, 
		187 , 203, 48, 87, 78, 81, 158, 20, 77, 14, 165, 223, 
		222, 221, 186, 204, 177, 199, 182, 185, 206, 2, 215,
		220, 178, 10, 8, 219 };



char intensity[256] = { 32, 32, 32, 32, 46, 46, 45, 96, 96, 95, 39, 44, 58, 
		126, 59, 59, 61, 94, 94, 94, 34, 43, 43, 47, 47, 47, 47, 47, 47, 124, 
		60, 62, 62, 62, 62, 62, 62, 62, 62, 62, 41, 40, 40, 40, 40, 105, 105, 
		105, 105, 105, 92, 92, 92, 37, 37, 37, 37, 37, 108, 108, 108, 125, 123, 
		123, 123, 73, 91, 33, 93, 42, 99, 114, 114, 115, 115, 115, 115, 63, 
		120, 118, 118, 118, 118, 116, 116, 49, 49, 55, 55, 97, 122, 122, 122, 
		122, 102, 110, 110, 101, 117, 117, 117, 111, 111, 67, 74, 76, 76, 76, 
		76, 76, 106, 106, 106, 106, 50, 51, 51, 89, 89, 89, 89, 89, 89, 89, 
		70, 52, 53, 53, 53, 53, 53, 53, 84, 84, 84, 84, 84, 84, 84, 84, 84, 
		84, 84, 80, 83, 57, 57, 57, 57, 57, 57, 57, 57, 57, 119, 119, 119, 
		119, 119, 100, 98 , 107, 54, 54, 54, 54, 54, 54, 88, 88, 88, 88, 88, 
		71, 104, 104, 104, 104, 104, 104, 113, 112, 112, 112, 112, 112, 112, 
		86, 86, 90, 90, 90, 65, 65, 121, 121, 121, 121, 121, 69, 103, 103, 
		103, 103, 38, 85, 85, 85, 85, 35, 68, 79, 75, 75, 75, 75, 109, 56, 
		72, 36, 82, 82, 82, 82, 82, 82, 66, 64, 64, 64, 48, 87, 78, 81, 81, 
		81, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 
		77, 77, 77, 77 }; 

// ---------------------------------


TextualDriver::TextualDriver()
: VideoDriver("textual", "A GUI m_screen which thunks down to 80x25 text.", true)
{
	m_lfb = NULL;
	m_screen = smk_get_console_location();
}

TextualDriver::~TextualDriver()
{
}
	
int   TextualDriver::setMode( int width, int height, int depth )
{
	if ( m_lfb != NULL ) smk_mem_free( m_lfb );
	

	int pages = (width * height * sizeof(unsigned int)) / 4096 + 1;
	
	m_lfb = (unsigned int*)smk_mem_alloc( pages );
	if ( m_lfb == NULL ) return -1;
	
	m_width = width;
	m_height = height;
	m_depth = depth;

	// ------------------------------------
	Fill();

	
	return 0;
}

void* TextualDriver::getLFB( int *width, int *height, int *depth )
{
	printf("%s\n","request to get LFB" );

	*width = m_width;
	*height = m_height;
	*depth = m_depth;

	return m_lfb;
}

int	TextualDriver::init()
{
	addMode( 640, 480, RGB32 );
	addMode( 800, 600, RGB32 );
	addMode( 1024, 768, RGB32 );
	return 0;
}

int	TextualDriver::shutdown()
{
	if ( m_lfb != NULL ) 
	{
		void *data = m_lfb;
		m_lfb = NULL;
		smk_mem_free( data );
	}
	

	return 0;
}
 


int	TextualDriver::sync( int x, int y, int width, int height )
{
   if ( m_lfb == NULL ) return -1;
   if ( m_screen == NULL ) return -1;

   int left = x;
   int top = y;
   int right = x + width;
   int bottom = y + height;
   
   // Fit
   if ( left < 0 ) left = 0; if ( left >= m_width ) left = m_width-1;
   if ( top < 0 ) top = 0;   if ( top >= m_height ) top = m_height-1;
   if ( right < 0 ) right = 0;   if ( right >= m_width ) right = m_width-1;
   if ( bottom < 0 ) bottom = 0; if ( bottom >= m_height ) bottom = m_height-1;
   
   
   int pix_across = m_width / 80;
   int pix_down = m_height / 25;
   int pix_area = pix_across * pix_down; 
   
   // Convert to the text m_screen.
   left = left / pix_across;
   if ( (right % pix_across) == 0 ) right = right / pix_across;
   								else right = right / pix_across + 1;
   top = top / pix_down;
   if ( (bottom % pix_down) == 0 )  bottom = bottom / pix_down;
   								else bottom = bottom / pix_down + 1;
   
	// Do it..
   for ( int x = left; x < right; x++ )
   	for ( int y = top; y < bottom; y++ )
	{
		float average_R = 0;
		float average_G = 0;
		float average_B = 0;

		// ----------- work out the average colour in the square.
		for ( int i = 0; i < pix_across; i++ )
		 for ( int j = 0; j < pix_down; j++ )
		 {
			int lfb_x = x * pix_across + i;
			int lfb_y = y * pix_down + j;
			
			unsigned int col = m_lfb[ lfb_y * width + lfb_x ];

			int R = (col >> 16) & 0xFF;
			int G = (col >> 8) & 0xFF;
			int B = (col) & 0xFF;
			
			
	   		average_R += R;
	   		average_G += G;
	   		average_B += B;
		 }
		// ------------
		
		average_R = average_R / pix_area;	
		average_G = average_G / pix_area;	
		average_B = average_B / pix_area;	

		if ( average_R < 0 ) average_R = 0;
		if ( average_G < 0 ) average_G = 0;
		if ( average_B < 0 ) average_B = 0;

		if ( average_R > 255 ) average_R = 255;
		if ( average_G > 255 ) average_G = 255;
		if ( average_B > 255 ) average_B = 255;
		
		// Get the max...
		int col = (int)average_R;
		if ( average_G >  col ) col = (int)average_G;
		if ( average_B >  col ) col = (int)average_B;
		

		// the colour.
		if ( col < 85 ) m_screen[ y * 160 + (x*2) + 1 ] = 0x8;
		else if ( col < 170 ) m_screen[ y * 160 + x*2 + 1 ] = 0x7;
		else m_screen[ y * 160 + x*2 + 1 ] = 0xF;


		// the character
		m_screen[ y * 160 + x*2 ] = intensity[ col ];
	}

		
	return 0;
}

// ------------------------------------------------------------


void TextualDriver::Fill()
{
	if ( m_lfb == NULL ) return;
	
	int offset = m_width * sizeof(unsigned int);
	
	for ( int i = 0; i < m_height; i++ )
		memset( ((char*)m_lfb) + i * offset, 0, offset );
}


// ------------------------------------------------------------

void TextualDriver::Thunk( char *m_screen, unsigned int* lfb, int width, int height )
{
   if ( lfb == NULL ) return;
   if ( m_screen == NULL ) return;
   if ( width < 0 ) return;	
   if ( height < 0 ) return;

   if ( width > 1024 ) return;
   if ( height >  768 ) return;
   
   
   int pix_across = width / 80;
   int pix_down = height / 25;
   int pix_area = pix_across * pix_down; 
   
   
   for ( int x = 0; x < 80; x++ )
   	for ( int y = 0; y < 25; y++ )
	{
		float average_R = 0;
		float average_G = 0;
		float average_B = 0;

		// ----------- work out the average colour in the square.
		for ( int i = 0; i < pix_across; i++ )
		 for ( int j = 0; j < pix_down; j++ )
		 {
			int lfb_x = x * pix_across + i;
			int lfb_y = y * pix_down + j;
			
			unsigned int col = lfb[ lfb_y * width + lfb_x ];

			int R = (col >> 16) & 0xFF;
			int G = (col >> 8) & 0xFF;
			int B = (col) & 0xFF;
			
			
	   		average_R += R;
	   		average_G += G;
	   		average_B += B;
		 }
		// ------------
		
		average_R = average_R / pix_area;	
		average_G = average_G / pix_area;	
		average_B = average_B / pix_area;	

		if ( average_R < 0 ) average_R = 0;
		if ( average_G < 0 ) average_G = 0;
		if ( average_B < 0 ) average_B = 0;

		if ( average_R > 255 ) average_R = 255;
		if ( average_G > 255 ) average_G = 255;
		if ( average_B > 255 ) average_B = 255;
		
		// Get the max...
		int col = (int)average_R;
		if ( average_G >  col ) col = (int)average_G;
		if ( average_B >  col ) col = (int)average_B;
		

		// the colour.
		if ( col < 85 ) m_screen[ y * 160 + (x*2) + 1 ] = 0x8;
		else if ( col < 170 ) m_screen[ y * 160 + x*2 + 1 ] = 0x7;
		else m_screen[ y * 160 + x*2 + 1 ] = 0xF;


		// the character
		m_screen[ y * 160 + x*2 ] = intensity[ col ];
	}
   
}




