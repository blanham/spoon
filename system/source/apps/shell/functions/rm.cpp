#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "rm.h"

#include "../Shell.h"
#include <spoon/storage/File.h>

struct function_info info_rm = 
{
   "delete a file.",
   "rm [file]\n"
   "This will delete a file. careful.",
   rm_main
};


 int rm_main( int argc, char **argv )
 {
    if ( argc != 2 ) return -1;

	int success = File::Delete( argv[1] );

	if ( success != 0 )
		printf("%s\n","Deletion of file failed.");
	else
		printf("%s\n","success.");
	
   
   return 0;
 }
 
