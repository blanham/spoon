#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "dmesg.h"

#include "../Shell.h"

struct function_info info_dmesg = 
{
   "kernel's dmesg buffer",
   "dmesg\n"
   "prints the kernel's dmesg buffer",
   dmesg_main
};


 int dmesg_main( int argc, char **argv )
 {
	int size = smk_get_dmesg_size();

	printf("%s%i\n","dmesg size = ", size );

	char *data = (char*)malloc( size );

	if ( smk_get_dmesg( data, size ) < 0 )
	{
		printf("%s\n","failure to get dmesg" );
		free( data );
		return -1;
	}

	// -----------
	
	char *position = data;
	
		int line_count = 0;
		int char_count = 0;
		
	for ( int i = 0; i < size; i++ )
	{
		switch ( *position )
		{
			case '\n':
				line_count += 1;
				char_count  = -1;
			default:
				char_count += 1;
				printf("%c", *position );
				break;
		}

		if ( char_count == 80 )
		{
			line_count += 1;
			char_count = 0;
		}

		if ( line_count == 20 )
		{
			line_count = 0;
		    shell->getch();
		}

		position++;
	}


	// -----------
	
	free( data );
		 
   return 0;
 }
 




