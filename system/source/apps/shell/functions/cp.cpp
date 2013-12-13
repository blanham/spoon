#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "cp.h"

#include <spoon/storage/File.h>

struct function_info info_cp = 
{
   "copy a file",
   "cp [source] [dest]\n"
   "This will copy the file (source) to the destination FILE! "
   "NB: *** It needs a filename at the moment. *** ",
   cp_main
};


 int cp_main( int argc, char **argv )
 {
    if ( argc != 3 ) return -1;

    File *source = new File( argv[1] );
    if ( source->Open() < 0 )
     {
       delete source;
       printf("%s%s\n","Unable to open file: ", argv[1] );
       return -1;
     }

	File::Delete( argv[2] );
	File::Create( argv[2] );

    File *dest = new File( argv[2] );
    if ( dest->Open() < 0 )
     {
       delete source;
       delete dest;
       printf("%s%s\n","Unable to open/delete file: ", argv[2] );
       return -1;
     }

	
    char *buffer = (char*)malloc(16384);
    
	int bad = 0;
	
	int size = source->Size();
	while ( size > 0 )
	{
	   int len = 16384;
	   if ( size < len ) len = size;


	   if ( source->Read( buffer, len ) != len )
	    {
	      printf("%s(%i%s)\n", "error reading source. ", size, " left");
		  bad = 1;
	      break;
	    }
	    
	   if ( dest->Write( buffer, len ) != len )
	   {
		  printf("%s(%i%s)\n","unable to write to destination.", size, " left");
		  bad = 1;
		  break;
	   }
	      
	   size -= len; 
	}
	
     free( buffer );	

    source->Close();
    dest->Close();
    delete source;
    delete dest;
   
	if ( bad == 0 ) printf("%s\n","ok.");
   
   return 0;
 }
 
