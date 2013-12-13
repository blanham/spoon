#include "FileSystem.h"

#include <spoon/ipc/Messenger.h>




FileSystem::FileSystem( const char *code, const char *description, VFS::Device *dev )
{
	_code = strdup(code);
	_description = strdup(description);
	_dev = dev;
}
 

FileSystem::~FileSystem()
{
      if ( _code != NULL ) free( _code );
      if ( _description != NULL ) free( _description );
}





char *FileSystem::GetCode() { return _code; }
char *FileSystem::GetDescription() { return _description; }
VFS::Device *FileSystem::GetDevice() { return _dev; }





int FileSystem::Test() 
{ 
	return -1; 
}

int FileSystem::Init() 
{ 
	return -1; 
}

int FileSystem::Shutdown() 
{ 
	return -1; 
}
  
// -----------------------------------------

int FileSystem::Create( char *node )
{
	return -1;
}

void *FileSystem::Open( const char *node ) 
{ 
	return NULL; 
}

int FileSystem::Read( int64 pos, void *buffer, int len, void *data )
{
	return -1; 
}


int FileSystem::Write( int64 pos, void *buffer, int len, void *data )
{ 
	return -1; 
}

int FileSystem::Delete( char *node )
{
	return -1;
}

void FileSystem::Close( void *data )
{ 
	return; 
}


int FileSystem::Stat( struct stat *st, void *data )
{ 
	return -1; 
}

int FileSystem::List( char *node, Message **list )
{
	*list = NULL;
	return -1;
}


int FileSystem::MakeDirectory( char *node )
{
	return -1;
}


int FileSystem::DeleteDirectory( char *node )
{
	return -1;
}



// ***********************************************************************************

int FileSystem::dev_read( int64 pos, void *buffer, int len )
{
	printf("%s( %i, %x, %i )\n", "dev_read", (int)pos, buffer, len );
		
	Message *reply = NULL;
	 Message *re = new Message(READ);
	          re->AddInt32("_len", len );
		  	  re->AddInt64("_seek", pos );
	 
		reply = Messenger::SendReceiveMessage( _dev->getPid(), _dev->getPort(), re, DEVICE_TIMEOUT );
		if ( reply == NULL )
		{
			delete re;
			return -1;
		}

	 delete re;

	 if ( reply == NULL ) return -1;
	 if ( reply->what != OK ) 
	 {
	 	delete reply;
	 	return -1;
	 }
	
	 int rc = -1;
 	 if ( reply->FindInt32("_rc", &rc) != 0 ) 
	 {
	 	delete reply;
	 	return -1;
	 }
	 
	 void *data;
 	 int size;
 	 if ( reply->FindData("_data", RAW_TYPE, (const void**)&data, &size ) != 0 ) 
	 {
	   	delete reply; 
	   	return -1;
	 }

     if ( (rc != size) || (rc > len) ) 
	 {	
	 	// something's playing sneaky buggers
	   delete reply;
	   return -1;
	 }

	memcpy( buffer, data, len );
	delete reply;
	return rc;
}
	

int FileSystem::dev_write( int64 pos, void *buffer, int len )
{
	printf("%s( %i, %x, %i )\n", "dev_write", (int)pos, buffer, len );
		
	Message *reply = NULL;
	 Message *re = new Message(WRITE);
	          re->AddInt32("_len", len );
			  re->AddInt64("_seek", pos );
			  re->AddData("_data", RAW_TYPE, buffer, len );
	 
		reply = Messenger::SendReceiveMessage( _dev->getPid(), _dev->getPort(), re, DEVICE_TIMEOUT );
		if ( reply == NULL )
		{
			delete re;
			return -1;
		}

	 delete re;

	 if ( reply == NULL ) return -1;
	 if ( reply->what != OK ) 
	 {
	 	delete reply;
	 	return -1;
	 }
	
	 int rc = -1;
 	 if ( reply->FindInt32("_rc", &rc) != 0 ) 
	 {
	 	delete reply;
	 	return -1;
	 }
	 delete reply;
	 return rc;
}


