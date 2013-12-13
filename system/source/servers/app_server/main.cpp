#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppServer.h"


int main( int argc, char *argv[] )
{
   AppServer *app = new AppServer();
              app->Run();
   delete app;

   return 0;
}

