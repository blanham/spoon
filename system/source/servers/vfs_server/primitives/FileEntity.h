#ifndef _VFS_FILEENTITY_H
#define _VFS_FILEENTITY_H

#include "Entity.h"

namespace VFS 
{

class Mount;

class FileEntity : public Entity
{
	public:
		FileEntity( const char *name,	int pid, int fd, Mount *mnt, void *data );
		virtual ~FileEntity();


		long long Read( void* data, long long len ); 
		long long Write( void *data, long long len );
		int Stat( struct stat *st );
		int Close();


		
		Mount* 	getMount();
		void* 	getData();


	private:
		Mount* 	  m_mount;
		void*	  m_data;
};

};

#endif

