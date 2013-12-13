#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../flist.h"
#include "tan.h"


struct function_info info_tan = 
{
   "tan trigonometric tan function",
   "tan [degrees]",
   tan_main
};


 int tan_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
   float a = atoi( argv[1] ) / ( 2 * 3.1415926);
   float ans = tan(a);

	printf("%s(%i) = %f\n", "tan", a, ans );
	
   return 0;
 }
 
