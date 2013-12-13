#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>

#include "DeviceEntity.h"
#include "../io/Device.h"

namespace VFS {

DeviceEntity::DeviceEntity( const char *name, int pid, int fd, Device* device )
: Entity( name, pid, fd )
{
	m_device = device;
}

DeviceEntity::~DeviceEntity()
{
}


long long DeviceEntity::Read( void* data, long long len )
{
	 Message *re = new Message(READ);
	          re->AddInt32("_len", len );
			  re->AddInt64("_seek", getPosition() );
		 

	Message *reply = Messenger::SendReceiveMessage( getDevice()->getPid(), 
													getDevice()->getPort(),
													re );
	delete re;
	if ( reply == NULL ) return -1; 


	  int rc;
	  if ( (reply->what != OK) || (reply->FindInt32("_rc", &rc) != 0) )
	  {
		delete reply;
		return -1;
	  }

	
	if ( rc > 0 ) Seek( getPosition() + rc );
	if ( rc < 0 ) 
	{
		delete reply;
		return -1;
	}


	int size;
	void *rd;

	if ( reply->FindData( "_data", RAW_TYPE, 0, (const void**)&rd, &size ) != 0 )
	{
		delete reply;
		return -1;
	}	
	
	memcpy( data, rd, size );
	delete reply;
	return rc;
}

long long DeviceEntity::Write( void *data, long long len )
{
	Message *re = new Message(WRITE);
	          re->AddInt32("_len", len );
			  re->AddInt64("_seek", getPosition() );
			  re->AddData("_data", RAW_TYPE, data, len );
		 

	Message *reply = Messenger::SendReceiveMessage( getDevice()->getPid(), 
													getDevice()->getPort(),
													re );
	delete re;
	if ( reply == NULL ) return -1; 


	  int rc;
	  if ( (reply->what != OK) || (reply->FindInt32("_rc", &rc) != 0) )
	  {
		delete reply;
		return -1;
	  }

	delete reply;
	
	if ( rc > 0 ) Seek( getPosition() + rc );

	return rc;
}
	
		
int DeviceEntity::Stat( struct stat *st )
{
  Message *devMsg = new Message(STAT);

  	Message *reply = Messenger::SendReceiveMessage(getDevice()->getPid(), getDevice()->getPort(), devMsg );

	delete devMsg;

	if ( reply == NULL ) return -1;
	
	int size;
    void *newst;
	
	if ( reply->FindData("_stat", RAW_TYPE, 0, (const void**)(&newst), &size ) != 0 )
	{
		delete reply;
		return -1;
	};


	memcpy( st, newst, sizeof( struct stat ) );
	delete reply;
	return 0;
}
		
int DeviceEntity::Close()
{
	return 0;
}



Device* 	DeviceEntity::getDevice()
{
	return m_device;
}

};




