#include <types.h>
#include <spoon/storage/DeviceNode.h>

#ifndef _RamNode_H
#define _RamNode_H


class	RamNode	:	public DeviceNode
{
	public:
		RamNode( char *path, int megabytes );
       ~RamNode();


       virtual int read( int64 pos, void *buffer, int len );
       virtual int write( int64 pos, void *buffer, int len );
	   virtual int stat( struct stat* );

	private:
		char *disk;
		unsigned int disk_size;

};


#endif

