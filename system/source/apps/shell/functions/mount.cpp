#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "mount.h"

#include <spoon/storage/VFSControl.h>

struct function_info info_mount = 
{
   "mount a device",
   "mount [device] [point] [fs]\n"
   "This will attempt to mount the device to the point specified "
   "using the file system specified.",
   mount_main
};


 int mount_main( int argc, char **argv )
 {
    if ( argc != 4 ) return -1;


    VFSControl *vfs = new VFSControl();
    int ans = vfs->Mount( argv[1], argv[2], argv[3] );
    delete vfs;

      if ( ans == 0 )
       printf("%s\n","Successfully mounted device.");
      else 
       printf("%s\n","Failed to mount device.");

   return 0;
 }
 
