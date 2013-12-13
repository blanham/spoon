#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "FileEntity.h"
#include "../Mount.h"

namespace VFS {

FileEntity::FileEntity( const char *name, int pid, int fd, Mount *mnt, void *data )
: Entity( name, pid, fd )
{
	m_mount = mnt;
	m_data = data;
}

FileEntity::~FileEntity()
{
}


long long FileEntity::Read( void* data, long long len )
{
	printf("%s%i, %i\n","Read complete: ", (int)getPosition(), (int)len);
	 int ans =  getMount()->getFS()->Read( getPosition(), data, (int)len, getData() );
	printf("%s%i\n","Read returned: ", ans );
	 if ( ans < 0 )  return -1;
 	 
	 Seek( getPosition() + ans );
	 return ans;
}

long long FileEntity::Write( void *data, long long len )
{
	printf("%s%i, %i\n","Write complete: ", getPosition(), (int)len);
	 int ans =  getMount()->getFS()->Write( getPosition(), data, (int)len, getData() );
	printf("%s%i\n","Write returned: ", ans );
		
	 if ( ans < 0 )  return -1;
 	 
	 Seek( getPosition() + ans );
	 return ans;
}
	
		
int FileEntity::Stat( struct stat *st )
{
	printf("%s\n","stat has begun.");
  int ans = getMount()->getFS()->Stat( st, getData() );
	printf("%s%i, %i\n","Stat returned: ", ans, st->st_size );
  if ( ans != 0 ) return -1;

  return 0;
}
		
int FileEntity::Close()
{
    getMount()->getFS()->Close( getData() );
	return 0;
}


		

Mount* 	FileEntity::getMount()
{
	return m_mount;
}

void* 	FileEntity::getData()
{
	return m_data;
}

};




