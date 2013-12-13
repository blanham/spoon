#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "KBDDriver.h"

int main( int argc, char *argv[] )
{
   char *proc = NULL;
   
   if ( smk_request_iopriv() != 0 ) 
   {
	 printf("%s\n","iopriv levels were not granted by the kernel.");
	 return -1;
   }
   
   
   if ( argc == 2 ) 
   {
     proc = argv[1];
     printf("%s%s\n",
           "kbd driver is sending events to ",
	   proc );
   }
   else
   {
     printf("%s\n","kbd driver is sending events to the default "
                   "input_server.");
   }

   


	KBDDriver *driver = new KBDDriver( proc );
		   driver->Run();
 	    delete driver;
 


    return 0;
}

