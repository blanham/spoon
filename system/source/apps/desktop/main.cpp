#include <types.h>
#include <stdio.h>
#include <kernel.h>

#include "DesktopApp.h"


int main( int argc, char *argv[] )
{

   DesktopApp *app = new DesktopApp();
            app->Run();
     delete app;

   return 0;
}

