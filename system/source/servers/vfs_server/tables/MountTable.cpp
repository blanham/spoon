#include <string.h>
#include <spoon/collections/List.h>
#include "MountTable.h"

#include "../Mount.h"

MountTable::MountTable()
{ 
}

MountTable::~MountTable()
{
}


   // ------------------------------------------

VFS::Mount* MountTable::GetMount( char *node )
{
    for ( int i = 0; i < CountMounts(); i++ )
	{
	   VFS::Mount *dog = MountAt(i);
	   if ( strcmp( dog->getPoint(), node ) == 0 ) return dog;
	}

	return NULL;
}

// node must match format:  /path/path/path/path/path[/]
VFS::Mount *MountTable::MatchMount( char *node )
{
 VFS::Mount *best = NULL;
 int best_len = -1;
 int node_len = strlen( node );
       
  for ( int i = 0; i < CountMounts(); i++ )
  {
    VFS::Mount *dog = MountAt(i);
    int dog_len = strlen( dog->getPoint() );
	    
    if ( node_len >= dog_len )		// worth it?
     if ( strncmp( dog->getPoint(), node, dog_len ) == 0 ) // matches?
     {
    	if ( dog_len >= best_len  ) // more appropriate?
		{
		  best = dog;
		  best_len = dog_len;
		}
     }
	    
  }

 return best;
}



   // ******************************************
   
int MountTable::CountMounts()
{ 
	return m_mounts.count(); 
}
      
   
void MountTable::AddMount( VFS::Mount *mnt )
{
    m_mounts.add( mnt );
}

VFS::Mount* MountTable::MountAt( int i ) 
{ 
	return (VFS::Mount*)m_mounts.get(i); 
}

VFS::Mount* MountTable::RemoveMount( int i )
{
	VFS::Mount *sam = MountAt(i);
    if ( sam == NULL ) return NULL;
    if ( RemoveMount( sam ) ) return sam;
    return NULL;
}
     
bool MountTable::RemoveMount( VFS::Mount *sam )
{
    return m_mounts.remove( sam );
}





