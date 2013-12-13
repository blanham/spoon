#ifndef _VFS_ENTITY_H
#define _VFS_ENTITY_H


namespace VFS 
{

class Entity
{
	public:
		Entity( const char *name, int pid, int fd );
		virtual ~Entity();


		virtual long long Read( void* data, long long len ); 
		virtual long long Write( void *data, long long len );
		virtual long long Seek( long long position );
		virtual int Stat( struct stat *st );
		virtual int Close();


		const char *getName();
		int 		getPid();
		int 		getFd();
		long long 	getPosition();


	private:
		char *m_name;
		int m_pid;
		int m_fd;
		long long m_position;
};

};

#endif

