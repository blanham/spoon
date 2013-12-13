#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Entity.h"

namespace VFS {

Entity::Entity( const char *name, int pid, int fd )
{
	if ( name != NULL ) m_name 	= strdup( name );
				   else m_name = NULL;
	m_pid 	= pid;
	m_fd 	= fd;
	m_position = 0;
}

Entity::~Entity()
{
	if ( m_name != NULL ) free( m_name );
}


long long Entity::Read( void* data, long long len )
{
	return -1;
}

long long Entity::Write( void *data, long long len )
{
	return -1;
}
	
long long Entity::Seek( long long position )
{
	if ( position < 0 ) return -1;

	struct stat st;
	if ( Stat( &st ) != 0 ) return -1;
	if ( position < st.st_size ) m_position = position;
	return -1;
}
		
int Entity::Stat( struct stat *st )
{
	return -1;
}
		
int Entity::Close()
{
	return -1;
}


		
const char *Entity::getName()
{
	return m_name;
}

int 	Entity::getPid()
{
	return m_pid;
}

int 	Entity::getFd()
{
	return m_fd;
}

long long Entity::getPosition()
{
	return m_position;
}


};




