#include <types.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <spoon/storage/DeviceNode.h>
#include <spoon/storage/StorageDefs.h>
#include "RamNode.h"


#ifdef _BOOTDISK_
extern void *_DISK;
extern unsigned int _DISK_SIZE;
#endif

RamNode::RamNode( char *path, int megabytes )
: DeviceNode( path )
{
      #ifdef _BOOTDISK_
        disk = (char*)&_DISK;
		disk_size = (unsigned int)&_DISK_SIZE;
        printf("%s%i%s(%i %s)\n",
			   "ramdisk compiled with builtin image: ",
			   disk_size,
			   " bytes ",
			   disk_size / (1024 * 1024),
			   "MB"
			   );
      #else
        printf("%s%i\n","ramdisk allocating ", megabytes, " MB of ram.");
		disk = (char*)malloc( megabytes * 1024 * 1024 );
		disk_size = megabytes * 1024 * 1024;
		strcpy( disk, "RamNode driver for spoon\0" );
      #endif
}

RamNode::~RamNode()
{
}


int RamNode::read( int64 pos, void *buffer, int len )
{
	int count;
	
	if ( pos < 0 ) return -1;
	if ( pos >= disk_size ) return -1;
	
//	printf("%s%i,%i\n","ramdisk read request: ", (int)(pos / 512), len );
	
	count = len;
	if ( (pos + len) > disk_size ) count = disk_size - pos;
	
	memcpy( buffer,  (char*)(disk + pos), count );

	return count;
}


int RamNode::write( int64 pos, void *buffer, int len )
{
	int count;

	if ( pos < 0 ) return -1;
	if ( pos >= disk_size ) return -1;

//	printf("%s%i,%i\n","ramdisk write request: ", (int)(pos / 512), len );
	
	count = len;
	if ( (pos + len) > disk_size ) count = disk_size - pos;
	
    memcpy( (char*)((uint32)disk + (int)pos),  buffer, count );

	return count;
}


int RamNode::stat( struct stat* st )
{
	st->st_size = disk_size;
	st->st_mode = 0;
	st->st_dev  = 0;
	return 0;
}

