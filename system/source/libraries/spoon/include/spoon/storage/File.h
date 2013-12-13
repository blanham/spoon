#ifndef _SPOON_STORAGE_FILE_H
#define _SPOON_STORAGE_FILE_H

#include <types.h>

#include <spoon/storage/Statable.h>
#include <spoon/storage/StorageDefs.h>

#include <spoon/io/IOStream.h>

/** \ingroup storage
 *
 *
 */
class File : public IOStream, public Statable
{
   public:
     File( const char *node );
     virtual ~File();


	 static int Create( const char *node );
	 static int Delete( const char *node );
     static bool Exists( const char *node );


	 // IOStream
		virtual int Open();
		virtual int Write(const void *buf, int len );
		virtual int Read(void *buf, int len );
		virtual int Close();

		

	// Statable
     int Stat( struct stat *st );


     int64 Size();

     int64 Seek( int64 pos );	// 'cos hard disks are big
     

   		bool Eject();
		bool Loaded();


     int fd() { return _fd; }
     char *node() { return _node; }

   private:
      int _fd;
      char *_node;
};



#endif


