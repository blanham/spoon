#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "clear.h"


struct function_info info_clear = 
{
   "clear the screen",
   "clear\n"
   "clears the screen.",
   clear_main
};


 int clear_main( int argc, char **argv )
 {
   smk_conio_clear();
   return 0;
 }
 
