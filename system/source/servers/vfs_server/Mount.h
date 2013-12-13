#ifndef _VFS_MOUNT_H
#define _VFS_MOUNT_H

#include "fs/FileSystem.h"

namespace VFS {

class Mount
{
	public:
		Mount( const char *point, FileSystem *fs );
		virtual ~Mount();

		const char *getPoint();
		FileSystem *getFS();
		
		
	private:
		char *m_point;
		FileSystem *m_fs;
};


};

#endif

