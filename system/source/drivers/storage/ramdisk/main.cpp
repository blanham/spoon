#include <types.h>
#include <kernel.h>
#include "RAMDriver.h"


int main( int argc, char *argv[] )
{

  int mb = 8;

  if ( argc == 2 ) mb = atoi( argv[1] );
  
  if ( mb <= 0 ) mb = 8;
  if ( mb > 64 ) mb = 64;
  

  RAMDriver *ram = new RAMDriver( mb );
  	     ram->Run();
     delete ram;

  return 0;


  
}



