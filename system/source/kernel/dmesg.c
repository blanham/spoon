#include <varargs.h>
#include <puts.h>
#include <misc.h>

#include <atomic.h>


/// The kernel's dmesg buffer size 
#define DMESG_TMPSIZE 		512
#define DMESG_BUFSIZE 		16384		// Allowing for 201 lines on boot up.



static char dmesg_buffer[ DMESG_BUFSIZE ];
static int  dmesg_position = 0;

static unsigned int dmesg_lock = 0;



// Forward declarations 
int dmesg_int( char *buffer, int len,  int num );
int dmesg_uint( char *buffer, int len, unsigned int num );
int dmesg_hex( char *buffer, int len, unsigned int hex );
int dmesg_string( char *buffer, int len, const char *str );



/** Prints an integer into the dmesg_buffer */
int dmesg_int( char *buffer, int len,  int num )
{
	int val;
	
	if ( num >= 0 )
	{
		val = num;
		return dmesg_uint( buffer, len, val );
	}


	if ( dmesg_string( buffer, len, "-" ) == len ) return len;
	val = -num;
	return dmesg_uint( buffer + 1, len - 1, val );
}

/** Prints an unsigned integer into the dmesg_buffer */

int dmesg_uint( char *buffer, int len, unsigned int num )
{
	char number[32];
	int i 	= 0;
	int tmp = num;
	int count = 0;
		
	
	while ( 1==1 )
	{
		number[i++] = (tmp % 10) + '0';
		if ( tmp == 0 ) break;
		tmp = tmp / 10;
		if ( tmp == 0 ) break;
	}
	
	while ( i-- > 0 )
	{
		if ( count >= len ) return len;
		buffer[ count++ ] = number[i];
	}
	
	return count;
}

/** Prints an unsigned hexadecimal number into the dmesg_buffer */
int dmesg_hex( char *buffer, int len, unsigned int hex )
{
	char number[64];
	int i 	= 0;
	unsigned int tmp = hex;
	char letter;
	int count = 0;
		

	while ( 1==1 )
	{
		letter = tmp % 16;

		if ( letter < 10 ) letter = letter + '0';
					else   letter = letter - 10 + 'A';
		
		number[i++] = letter;

		if ( tmp == 0 ) break;
		tmp = tmp / 16;
		if ( tmp == 0 ) break;
	}
	
	number[i++] = 'x';
	number[i++] = '0';
	
	while ( i-- > 0 )
	{
		if ( count >= len ) return len;
		buffer[ count++ ] = number[i];
	}
	
	return count;
}


/** Prints a string into the dmesg_buffer */
int dmesg_string( char *buffer, int len, const char *str )
{
	char *s = (char*)str;
	int count = 0;

	if ( str == NULL ) return dmesg_string( buffer, len, "NULL" );	// Cool.
		
	while ( *s != 0 )
	{
		if ( count >= len ) return len;
		buffer[ count++ ] = *s++;
	}
	
	return count;
}


//--------------


/** sdmesg is a printf-styled function that accepts multiple modifiers
 * and instructions.
 *
 * It currently supports 5 modifiers:
 *
 *   %! - print to screen.
 *   %s - string argument.
 *   %i - signed integer.
 *   %u - unsigned integer.
 *   %x - hexadecimal unsigned number.
 * 
 * So you see that it's a very basic printf function.
 *
 * \return 0
 */
int sdmesg( char *buffer, int max_len, char *format, va_list ap, int *flags )
{
   char *tmpBuf;
   int position = 0;
   int len;

   int modifier = 0;
   char msg[2];
  		msg[1] = 0; 
   
	// pointers & modified data.
	char *string;
	int  number;
	unsigned int unsigned_number;
	
	// ----------------------
	
	*flags = 0;

	// ----------------------

   while (*format)
   {
	 char letter = *format;
	      format++;

	 len = max_len - position;
	 tmpBuf = buffer + position;
		   
	 if ( modifier != 0 )
	 {
      	switch( letter ) 
		{
			case '!':
					*flags = *flags | 1;
					break;
						
        	case 's': 
					string = va_arg( ap, char* );
					position += dmesg_string( tmpBuf, len, string );  
					break;

       	    case 'i': 
					number = va_arg( ap, int );
					position += dmesg_int( tmpBuf, len, number ); 
					break;

       	    case 'u': 
					unsigned_number = va_arg( ap, unsigned int );
					position += dmesg_uint( tmpBuf, len, unsigned_number ); 
					break;

        	case 'x': 
					unsigned_number = va_arg( ap, unsigned int );
					position += dmesg_hex( tmpBuf, len, unsigned_number ); 
					break;

	  	}

	 	modifier = 0;
  	 }
	 else
	 {
		switch( letter )
		{
			case '%':
					modifier = 1;
					break;
				
			default:
				msg[0] = letter;
				position += dmesg_string( tmpBuf, len, msg );
				break;
		}
	 }
   }

   if ( position < max_len ) buffer[ position ] = 0;
   
   return position;
}


/** dmesg thunks downn to sdmesg and then transfers the 
 * temporary buffer into the kernel's main buffer.
 */

int dmesg( char *format, ... )
{
	va_list ap;
	char buffer[ DMESG_TMPSIZE ];
	int rc;
	int i;
	int flags;

	va_start( ap, format );

		rc = sdmesg( buffer, DMESG_TMPSIZE, format, ap, &flags );
	
	va_end( ap );

	// Copy into the buffer.

	acquire_spinlock( &dmesg_lock );
	
		for ( i = 0; i < rc; i++ )
			dmesg_buffer[ (dmesg_position + i) % DMESG_BUFSIZE ] = buffer[i];

		dmesg_position = (dmesg_position + rc) % DMESG_BUFSIZE;

		// Print to screen?
		if ( (flags & 1) == 1 ) puts( buffer );
		//else puts(buffer);
		
	release_spinlock( &dmesg_lock );
	
	return rc;
}

 

/** dmesg_xy thunks downn to sdmesg and then outputs the
 * message to a location on screen.
 *
 * The buffer is not transfered into the kernel's dmesg.
 */

int dmesg_xy( int x, int y, char *format, ... )
{
	va_list ap;
	char buffer[ DMESG_TMPSIZE ];
	int rc;
	int a = x;
	int b = y;
	int flags;

	va_start( ap, format );
		rc = sdmesg( buffer, DMESG_TMPSIZE, format, ap, &flags );
	va_end( ap );

	puts_xy( &a, &b, buffer );
	return rc;
}

 


/** Copy the dmesg buffer into the provided location
 * of the given length. If len is less than the current
 * size of the dmesg_buffer, then only len bytes are
 * returned. If the current size of the dmesg_buffer is
 * less than the given len, then only the current size
 * is returned.
 *
 * \return The number of bytes copied into buffer.
 */
int get_dmesg( void *buffer, int len )
{
	acquire_spinlock( &dmesg_lock );
		int size = ( len < dmesg_position ) ? len : dmesg_position;
		memcpy( buffer, dmesg_buffer, size );
	release_spinlock( &dmesg_lock );
	return size;
}


/** Returns the size of the dmesg_buffer */
int get_dmesg_size()
{
	return DMESG_BUFSIZE;
}




