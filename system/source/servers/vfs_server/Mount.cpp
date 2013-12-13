#include <stdlib.h>
#include <string.h>
#include "Mount.h"

namespace VFS {

Mount::Mount( const char *point, FileSystem *fs )
{
	m_point = strdup(point);
	m_fs = fs;
}

Mount::~Mount()
{
	if ( m_point != NULL ) free( m_point );

	m_fs->Shutdown();
	delete m_fs;
}

const char *Mount::getPoint()
{
	return m_point;
}

FileSystem *Mount::getFS()
{
	return m_fs;
}

};


