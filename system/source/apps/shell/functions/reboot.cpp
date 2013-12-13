#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "reboot.h"


struct function_info info_reboot = 
{
   "reboot",
   "reboot",
   reboot_main
};


 int reboot_main( int argc, char **argv )
 {
   smk_reboot();
   return 0;
 }
 
