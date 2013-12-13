#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../flist.h"
#include "test.h"


struct function_info info_test = 
{
   "ignore. used for testing",
   "test [maybe]",
   test_main
};

char *message = "Hello from the shell!";

 int test_main( int argc, char **argv )
 {
   return 0;
 }
 
