#include <stdlib.h>
#include <string.h>

#include "DirectoryEntity.h"
#include "../Mount.h"

namespace VFS {

DirectoryEntity::DirectoryEntity( const char *name, int pid, int fd, Mount *mnt, void *data )
: Entity( name, pid, fd )
{
	m_mount = mnt;
	m_data = data;
}

DirectoryEntity::~DirectoryEntity()
{
}


long long DirectoryEntity::Read( void* data, long long len )
{
	return -1;
}

long long DirectoryEntity::Write( void *data, long long len )
{
	return -1;
}
	
		
int DirectoryEntity::Stat( struct stat *st )
{
	return -1;
}
		
int DirectoryEntity::Close()
{
	return 0;
}


		

Mount* 	DirectoryEntity::getMount()
{
	return m_mount;
}

void* 	DirectoryEntity::getData()
{
	return m_data;
}

};




