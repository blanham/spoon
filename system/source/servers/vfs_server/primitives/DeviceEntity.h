#ifndef _VFS_DEVICEENTITY_H
#define _VFS_DEVICEENTITY_H

#include "Entity.h"
#include "../io/Device.h"

namespace VFS 
{


class DeviceEntity : public Entity
{
	public:
		DeviceEntity( const char *name,	int pid, int fd, Device* device );
		virtual ~DeviceEntity();


		long long Read( void* data, long long len ); 
		long long Write( void *data, long long len );
		int Stat( struct stat *st );
		int Close();


		
		Device* 	getDevice();


	private:
		Device*   m_device;
};

};

#endif

