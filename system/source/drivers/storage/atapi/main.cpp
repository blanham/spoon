#include <types.h>
#include <kernel.h>
#include <stdio.h>


#include "ATAPI_Driver.h"

int main( int argc, char *argv[] )
{
   if ( smk_request_iopriv() != 0 ) 
   {
	 printf("%s\n","iopriv levels were not granted by the kernel.");
	 return -1;
   }
 

    ATAPI_Driver *atapi = new ATAPI_Driver();
               atapi->Run();
        delete atapi;

   return 0;
}


