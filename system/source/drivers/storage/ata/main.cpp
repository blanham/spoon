#include <types.h>
#include <kernel.h>
#include <stdio.h>


#include "ATA_Driver.h"

int main( int argc, char *argv[] )
{

   if ( smk_request_iopriv() != 0 ) 
   {
	 printf("%s\n","iopriv levels were not granted by the kernel.");
	 return -1;
   }
 
 
		
    ATA_Driver *ata = new ATA_Driver();
               ata->Run();
        delete ata;

   return 0;
}


