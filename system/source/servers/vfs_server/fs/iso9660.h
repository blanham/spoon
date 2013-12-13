#ifndef _FS_ISO9660_H
#define _FS_ISO9660_H

#include "FileSystem.h"
#include "iso9660_defs.h"

extern FileSystem *create_iso9660( VFS::Device *dev );


class fs_iso9660 : public FileSystem
{
   public:
     fs_iso9660( VFS::Device *dev );
     virtual ~fs_iso9660();
     

   public:
	virtual int Init(); 
	virtual int Shutdown();

	virtual int Test();
   
   	virtual void *Open( const char *node ); 
	virtual int Read( int64 pos, void *buffer, int len, void *data );
	virtual int Write( int64 pos, void *buffer, int len, void *data );
	virtual void Close( void *data );

	virtual int Stat( struct stat *st, void *data );
	virtual int List( char *node, Message **msg );


   private:
        struct directory_record* findFile( const char *name );
	struct primary_volume_descriptor* getPrimary();

	struct directory_record* run_dirs(  char *match,
         					int block, 
						int offset, 
						int size );

	int insensitive_match( char *src, char *dest );

   private:
   	struct primary_volume_descriptor pvd;

};



#endif

