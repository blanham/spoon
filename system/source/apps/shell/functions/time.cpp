#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../flist.h"
#include "time.h"


struct function_info info_time = 
{
   "current system time",
   "time\n"
   "prints out the system time of the computer, not the CMOS.",
   time_main
};


 int time_main( int argc, char **argv )
 {
   int minutes;
   int hours;
   int seconds;
   char message[128];
   unsigned int t = time(NULL);


   	hours   = t / ( 60 * 60 );
	minutes = (t / 60) % 60;
   	seconds = t % 60;
   	
	sprintf(message,"%2i:%2i:%2i",
			hours,
			minutes,
			seconds );
	
	printf("%s\n", message );
   
   return 0;
 }
 
