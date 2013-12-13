#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "cat.h"

#include <spoon/storage/File.h>

struct function_info info_cat = 
{
   "display the contents of a file",
   "cat [file]\n"
   "This will display the contents of a file. If you want "
   "the contents displayed page by page, rather try the \"more\" "
   "command.",
   cat_main
};


 int cat_main( int argc, char **argv )
 {
    if ( argc != 2 ) return -1;

    File *file = new File( argv[1] );
    if ( file->Open() < 0 )
     {
       delete file;
       printf("%s%s\n","Unable to open file: ", argv[1] );
       return -1;
     }

     char *buffer = (char*)malloc(16384);
     
	int size = file->Size();
	while ( size > 0 )
	{
	   int len = 16384;
	   if ( size < len ) len = size;
	   if ( file->Read( buffer, len ) != len )
	    {
	      printf("%s\n", "Error reading file. ");
	      break;
	    }
	    
	   for ( int i = 0; i < len; i++ )
	   {
	    if ( isprint( buffer[i] ) != 0 )
	      printf("%c", buffer[i] );
	     else 
	      printf("%c", '.' );

	    if ( buffer[i] == '\n' ) printf("%s\n","" );
	   }
	      
	   size -= len; 
	}
	
     free( buffer );	

    file->Close();
    delete file;
   
    printf("\n");
   
   return 0;
 }
 
