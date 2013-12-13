#include <types.h>
#include <stdio.h>
#include <kernel.h>

#include "CommanderApp.h"


int main( int argc, char *argv[] )
{

   CommanderApp *app = new CommanderApp();
            app->Run();
     delete app;

   return 0;
}

