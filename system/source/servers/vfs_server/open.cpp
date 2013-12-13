#include "Mount.h"
#include "open.h"

#include "VFSServer.h"

#include "primitives/DeviceEntity.h"
#include "primitives/FileEntity.h"

VFS::Entity* open_device( int pid, const char *node )
{
	printf("%s\n","Looking for device.");
	VFS::Device* dev = device_table.getDevice( (char*)(node + strlen("/device/")) );
    if ( dev == NULL ) return NULL;

	printf("%s\n","Found file.");
	return new VFS::DeviceEntity( node, pid, file_table.UniqueFD( pid ), dev );
}

VFS::Entity* open_file( int pid, const char *node )
{
	printf("%s\n","Looking for mount.");
    VFS::Mount *mnt = mount_table.MatchMount( (char*)node );
    if ( mnt == NULL ) return NULL;

	printf("%s\n","Looking for Open.");
    void *data = mnt->getFS()->Open( (char*)(node + strlen(mnt->getPoint())) );
    if ( data == NULL ) return NULL;

	printf("%s\n","Found file.");
	return new VFS::FileEntity( node, pid, file_table.UniqueFD( pid ), mnt, data );
}
   


/** open the is major switching function. Depending on node,
 * the type of Entity is constructed and returned.
 */
VFS::Entity*  open( int pid, const char *node )
{
	printf("%s(%i,%s)\n","open: ", pid, node );

	// Determine what kind of Entity must be created.

		// Device Tree
	if ( strncmp( node, "/device/", strlen("/device/") ) == 0  )
	{
		return open_device( pid, node );
	}
		

		// Finally, a FileEntity.
	return open_file( pid, node );
}


	   








