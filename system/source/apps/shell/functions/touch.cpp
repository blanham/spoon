#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../flist.h"
#include "touch.h"

#include <spoon/storage/File.h>

struct function_info info_touch = 
{
   "create an empty file.",
   "touch [filename]",
   touch_main
};

 int touch_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
     
     int fd = File::Create( argv[1] );
	 if ( fd < 0 )
		printf("%s%s\n","unable to create file: ", argv[1]);
	 else
		printf("%s\n","success." );
   
	
   return 0;
 }
 
