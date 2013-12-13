#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "spit.h"


struct function_info info_spit = 
{
   "kernel spits out information",
   "spit",
   spit_main
};


 int spit_main( int argc, char **argv )
 {
   smk_spit();
	
   return 0;
 }
 
