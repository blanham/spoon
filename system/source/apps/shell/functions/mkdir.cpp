#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "mkdir.h"

#include "../Shell.h"
#include <spoon/storage/Directory.h>
#include <spoon/storage/File.h>

struct function_info info_mkdir = 
{
   "create a directory",
   "mkdir [path]\n"
   "This will create the directory specified by path.",
   mkdir_main
};


 int mkdir_main( int argc, char **argv )
 {
    if ( argc != 2 ) return -1;

	int success = Directory::Create( argv[1] );

	if ( success != 0 )
		printf("%s\n","Creation of directory failed.");
	else
		printf("%s\n","success.");
	
   
   return 0;
 }
 
