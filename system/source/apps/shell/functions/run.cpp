#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "run.h"

#include <spoon/app/System.h>

struct function_info info_run = 
{
   "execute a binary",
   "run [file] [parameters]\n"
   "This will run a file from disk. " ,
   run_main
};


 int run_main( int argc, char **argv )
 {
    if ( argc < 2 ) return -1;

	// Build up the command line parameters
     char *cmd_line = (char*)malloc(1024);
           cmd_line[0] = 0;
           for ( int i = 2; i < argc; i++ )
	     {
	       if ( i != 2 ) strcat( cmd_line, " " );
               strcat( cmd_line, argv[i] );
	     }


     int rc = System::Execute( argv[1], cmd_line );

     printf("%s%i\n","return code: ", rc );
  
     free( cmd_line );

   return 0;
 }
 
