#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "sleep.h"


struct function_info info_sleep = 
{
   "sleep for a few seconds",
   "sleep [seconds]",
   sleep_main
};


 int sleep_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
   int a = atoi( argv[1] );
   
   if ( a > 120 ) a = 120;  // cap it because I make stupid mistakes sometimes.
   
   if ( a > 0 ) smk_sleep( a );
	
   return 0;
 }
 
