#include <types.h>
#include "InputServer.h"


int main( int argc, char *argv[] )
{
   InputServer *input = new InputServer();
              input->Run();
   delete input;

   return 0;
}

