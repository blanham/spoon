#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "rmdir.h"

#include "../Shell.h"
#include <spoon/storage/Directory.h>

struct function_info info_rmdir = 
{
   "delete a directory",
   "rmdir [path]\n"
   "This will delete an empty directory.",
   rmdir_main
};


 int rmdir_main( int argc, char **argv )
 {
    if ( argc != 2 ) return -1;

	int success = Directory::Delete( argv[1] );

	if ( success != 0 )
		printf("%s\n","Deletion of directory failed. Is it empty?");
	else
		printf("%s\n","success.");
	
   
   return 0;
 }
 
