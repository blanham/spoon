#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

#include "TerminalApp.h"

int main( int argc, char *argv[] )
{

   char *file = NULL;
   if ( argc > 1 ) file = argv[1];
   
   TerminalApp *app = new TerminalApp( file );
                app->Run();
   delete app;
   
   return 0;
}

