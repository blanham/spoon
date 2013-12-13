#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "unmount.h"

#include <spoon/storage/VFSControl.h>

struct function_info info_unmount = 
{
   "unmount a device",
   "unmount [point]\n"
   "This will attempt to umount a file system point, if it is a "
   "mounted point.",
   unmount_main
};


 int unmount_main( int argc, char **argv )
 {
    if ( argc != 2 ) return -1;


    VFSControl *vfs = new VFSControl();
    int ans = vfs->Unmount( argv[1] );
    delete vfs;

      if ( ans == 0 )
       printf("%s\n","Successfully unmounted point.");
      else 
       printf("%s\n","Failed to unmount point.");

   return 0;
 }
 
