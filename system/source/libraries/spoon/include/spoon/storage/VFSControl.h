#ifndef _SPOON_STORAGE_VFS_CONTROL_H
#define _SPOON_STORAGE_VFS_CONTROL_H


/** \ingroup storage
 *
 *
 */
class VFSControl
{
   public:
     VFSControl();
     ~VFSControl();

      static int Mount( char *device, char *point, char *fs );
      static int Unmount( char *point );

   
	
};


#endif

