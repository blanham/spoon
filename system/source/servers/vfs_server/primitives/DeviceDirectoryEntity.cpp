#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "DeviceDirectoryEntity.h"
#include "../Mount.h"

namespace VFS {

DeviceDirectoryEntity::DeviceDirectoryEntity( const char *name, int pid, int fd )
: Entity( name, pid, fd )
{
}

DeviceDirectoryEntity::~DeviceDirectoryEntity()
{
}


long long DeviceDirectoryEntity::Read( void* data, long long len )
{
	return -1;
}

long long DeviceDirectoryEntity::Write( void *data, long long len )
{
	return -1;
}
	
		
int DeviceDirectoryEntity::Stat( struct stat *st )
{
	  	st->st_dev  = 0;
	  	st->st_size = 0;
		st->st_mode = S_IFDIR; 
		st->st_uid  = 0;
		st->st_gid  = 0;
		st->st_atime = 0; 
		st->st_mtime = 0;
		st->st_ctime = 0;
	
	return 0;
}
		
int DeviceDirectoryEntity::Close()
{
	return 0;
}



};




