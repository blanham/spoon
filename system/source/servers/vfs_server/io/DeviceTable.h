#ifndef _VFS_DEVICETABLE_H
#define _VFS_DEVICETABLE_H

#include <string.h>
#include <spoon/collections/List.h>

#include "Device.h"

namespace VFS
{


class DeviceTable : public List
{
  public:
    DeviceTable();
    virtual ~DeviceTable();

	int getNextDeviceID();

    Device* getDevice( char *node );
    Device* getDevice( int pid, int port );
	Device* getDevice( int did );
	
};

}

#endif


