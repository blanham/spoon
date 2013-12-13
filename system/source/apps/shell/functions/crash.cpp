#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "crash.h"


struct function_info info_crash = 
{
   "crash",
   "This causes a divide by zero exception",
   crash_main
};


 int crash_main( int argc, char **argv )
 { 
   int i = 0;
       i = 10 / i;
   return 0;
 }
 
