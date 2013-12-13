#include <types.h>
#include <lfb.h>


// External declarations

void lfb_clear();
void lfb_scroll();
void lfb_putc( int x, int y,  char c );
int  lfb_width();
int  lfb_height();

void screen_clear();
void screen_scroll();
void screen_putc( int x, int y, char c );
int  screen_width();
int  screen_height();


// .................. local variables

static int s_x = 0;	//< The cursor's X position.
static int s_y = 0; //< The cursor's Y position.

// .,................

/** Clear the screen, whether it's LFB or text mode */
void clear() 
{
    if ( using_lfb() == 1 )  lfb_clear();
						else screen_clear();

	s_x = 0;
	s_y = 0;
}


void scroll()
{
	if ( using_lfb() == 1 )  lfb_scroll();
						else screen_scroll();
}



void puts_xy( int *x, int *y, const char *s )
{
	int width = 0;
	int height = 0;
    long i = 0;

	// Obtain the coordinates.
	if ( using_lfb() == 1 )
	{
		width = lfb_width();
		height = lfb_height();
	}
	else
	{
		width = screen_width();
		height = screen_height();
	}
	

	// print the string. 
		
    while(s[i] != '\0') 
	{

		if ( *x >= width )
		{
			*x  = 0;
			*y += 1;
		}

		if ( *y >= height )
		{
			scroll();
			*y -= 1;
		}
			
			
		switch (s[i])
		{
			case '\n':
					*x  = 0;
					*y += 1;
					break;
				
			default:
					if ( using_lfb() == 1 )  lfb_putc( *x, *y, s[i] );
										else screen_putc( *x, *y, s[i] );
					*x += 1;
					break;
		}
		
		i += 1;
	}
}


void puts( const char *str )
{
	puts_xy( &s_x, &s_y, str );
}




