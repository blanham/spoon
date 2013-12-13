#ifndef _VFS_SERVER_MOUNTTABLE_H
#define _VFS_SERVER_MOUNTTABLE_H

#include <string.h>
#include <spoon/collections/List.h>

#include "../Mount.h"

class MountTable
{
  public:
    MountTable();
    ~MountTable();


	VFS::Mount *GetMount( char *node );
	VFS::Mount *MatchMount( char *node );


     int 	CountMounts();
     void 	AddMount( VFS::Mount *mnt );
	 VFS::Mount* MountAt( int i ) ;
	 VFS::Mount *RemoveMount( int i );
     bool 	RemoveMount( VFS::Mount *sam );

    
   private:
    List m_mounts;

};

    
#endif


