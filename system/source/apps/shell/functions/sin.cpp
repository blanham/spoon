#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../flist.h"
#include "sin.h"


struct function_info info_sin = 
{
   "sin trigonometric sin function",
   "sin [degrees]",
   sin_main
};


 int sin_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
   float a = atoi( argv[1] );
         a = a / ( 2 * 3.1415926 );

   float ans = sin(a);

	printf("%s(%i) = %f\n", "sin", a, ans );
	
   return 0;
 }
 
