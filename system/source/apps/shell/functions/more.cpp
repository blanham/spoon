#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "more.h"

#include "../Shell.h"
#include <spoon/storage/File.h>

struct function_info info_more = 
{
   "display the contents of a file per page",
   "more [file]\n"
   "This will display the contents of a file while waiting "
   "on user-input at every page. ",
   more_main
};


 int more_main( int argc, char **argv )
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
     
      int lines = 0;
      int chars = 0;
     
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
	    
	   	// PAGE BREAK;  
		   if ( buffer[i] != '\n' ) chars++;
		   else {
		          lines++;
			  chars = 0;
		        }
	           
		   if ( chars == 80 ) 
		      {
		        lines++;
			chars = 0;
		      }
		   
		   if ( lines == 24 ) 
		    {
		      printf("%s","Press any key to continue... [ q to quit ]" );
		      int c = shell->getch();
		      printf("%s\n","" );
		      lines = 0;
		      chars = 0;
		      if ( (c == 'q') || (c=='Q') ) 
		      {
		        lines = -1;
		      	break;
		      }
		    }
	    }
			
	   // ..................
	   
	   size -= len; 
	   if ( lines < 0 ) break;
	}
	
     free( buffer );	

    file->Close();
    delete file;
   
    printf("\n");
   
   return 0;
 }
 
