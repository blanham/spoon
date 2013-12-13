#include <stdlib.h>

#include <GuiServer.h>


int main( int argc, char *argv[] )
{

   GuiServer *gui = new GuiServer();
              gui->Run();
   delete gui;

   return 0;
}

