#ifndef _VFS_DEVICEDIRECTORYENTITY_H
#define _VFS_DEVICEDIRECTORYENTITY_H

#include "Entity.h"

namespace VFS 
{


class DeviceDirectoryEntity : public Entity
{
	public:
		DeviceDirectoryEntity( const char *name,	int pid, int fd );
		virtual ~DeviceDirectoryEntity();


		long long Read( void* data, long long len ); 
		long long Write( void *data, long long len );
		int Stat( struct stat *st );
		int Close();

};

};

#endif

