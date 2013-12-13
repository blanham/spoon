#ifndef _VFS_DIRECTORYENTITY_H
#define _VFS_DIRECTORYENTITY_H

#include "Entity.h"

namespace VFS 
{

class Mount;

class DirectoryEntity : public Entity
{
	public:
		DirectoryEntity( const char *name,	int pid, int fd, Mount *mnt, void *data );
		virtual ~DirectoryEntity();


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

