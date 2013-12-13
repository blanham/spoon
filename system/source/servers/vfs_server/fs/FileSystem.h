#ifndef _VFS_SERVER_FILE_SYSTEM_H
#define _VFS_SERVER_FILE_SYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <spoon/sysmsg.h>

#include <spoon/ipc/Message.h>

#include <spoon/storage/StorageDefs.h>


#include "../io/DeviceTable.h"

#define FS_UNSUPPORTED		-1
#define FS_EXACT		0
#define FS_SUPPORTED		1
#define FS_UNLIKED		2


#define DEVICE_TIMEOUT			TIMEOUT_10SEC


class FileSystem
{
   public:
     FileSystem( const char *code, const char *description, VFS::Device *dev );
    
     virtual ~FileSystem();

	
	 char *GetCode();
	
	 char *GetDescription();
	
	 VFS::Device* GetDevice();

	
	 virtual int Test();
	 virtual int Init();
	 virtual int Shutdown();
   
	 virtual int Create( char *node );
   	
	 virtual void *Open( const char *node );
	 virtual int  Read( int64 pos, void *buffer, int len, void *data );
	 virtual int  Write( int64 pos, void *buffer, int len, void *data );
	 virtual int  Delete( char *node );
	 virtual void Close( void *data );

	 
	 virtual int Stat( struct stat *st, void *data );
	 virtual int List( char *node, Message **list );

	 virtual int MakeDirectory( char *node );
	 virtual int DeleteDirectory( char *node );
	
	 

   //protected:
        int dev_read( int64 pos, void *buffer, int len );
        int dev_write( int64 pos, void *buffer, int len );

   protected:
		
   		char *_code;
		char *_description;
		VFS::Device *_dev;
     
};

#endif



