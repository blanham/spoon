#include <kernel.h>
#include <string.h>
#include <spoon/collections/List.h>

#include "DeviceTable.h"
#include "Device.h"


namespace VFS
{
		
DeviceTable::DeviceTable()
{
}
  
DeviceTable::~DeviceTable()
{
}


int DeviceTable::getNextDeviceID()
{
   int did = 1;
   Device* dev = NULL;

    do
    {
       did = (did + 1) % 32000;
       if ( did == 0 ) did = 1;
       dev = getDevice( did );
    }
    while (  dev != NULL );

    return did;
}
 


Device* DeviceTable::getDevice( char *node )
{
  for ( int i = 0; i < count(); i++ )
  {
    Device* dog = (Device*)get(i);
    if ( strcmp( dog->getName(), node ) == 0 ) return dog;
  }
 return NULL;
}

Device* DeviceTable::getDevice( int pid, int port )
{
  for ( int i = 0; i < count(); i++ )
  {
    Device* dog = (Device*)get(i);
    if ( (dog->getPid() == pid) &&
         (dog->getPort() == port ) ) return dog;
  }
 return NULL;
}


Device* DeviceTable::getDevice( int did )
{
  for ( int i = 0; i < count(); i++ )
  {
    Device* dog = (Device*)get(i);
    if ( (dog->getDeviceID() == did) ) return dog;
  }
 return NULL;
}
	



}


