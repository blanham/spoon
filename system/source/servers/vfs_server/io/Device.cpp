#include <stdlib.h>
#include <string.h>
#include "Device.h"

namespace VFS
{


Device::Device( const char *name, int pid, int port, int device_id )
{
	m_name = (char*)strdup(name);
	m_pid = pid;
	m_port = port;
	m_deviceID = device_id;
}

Device::~Device()
{
	if ( m_name != NULL ) free( m_name );
}

const char *Device::getName()
{
	return m_name;
}

int 		Device::getPid()
{
	return m_pid;
}

int 		Device::getPort()
{
	return m_port;
}

int 		Device::getDeviceID()
{
	return m_deviceID;
}
		


}

