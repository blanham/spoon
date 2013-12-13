#include <types.h>
#include <stdio.h>
#include "PS2Driver.h"



int main( int argc, char* argv[] )
{
   char *proc = "input_server";
   int mode = 0;

   if ( smk_request_iopriv() != 0 ) 
   {
	 printf("%s\n","iopriv levels were not granted by the kernel.");
	 return -1;
   }
 
   
   if ( argc == 2 ) 
   {
     proc = argv[1];
   }
   else if ( argc == 3 )
   {
     proc = argv[1];
	 mode = atoi( argv[2] );
   }


     printf("%s%s\n", "ps2mouse driver is sending events to ", proc );
     printf("%s%i\n", "ps2mouse driver mode: ", mode );

	 if ( mode < 0 ) mode = 0;
	 if ( mode > 3 ) mode = 3;

	 switch (mode)
	 {
		case 0: printf("%s\n","standard method" ); break;
		case 1: printf("%s\n","ye olde method" ); break;
		case 2: printf("%s\n","standard method without disabling keyb" ); break;
		case 3: printf("%s\n","standard method, 1 read per IRQ" ); break;
	 }

	PS2Driver *driver = new PS2Driver( mode, proc );
	           driver->Run();
	    delete driver;
  

  return 0;	
}



