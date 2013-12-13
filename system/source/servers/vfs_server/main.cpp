#include <types.h>
#include "VFSServer.h"


int main( int argc, char *argv[] )
{
   VFSServer *vfs = new VFSServer();
              vfs->Run();
   delete vfs;

   return 0;
}

