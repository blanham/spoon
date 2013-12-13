#ifndef _VFS_SERVER_FILETABLE_
#define _VFS_SERVER_FILETABLE_

#include <types.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Mount.h"

/*

   This is a hash table with a dynamic array which
   allows for:

     1. Fast lookup of files belonging to pid
     2. Memory of order.

*/

struct file_info
{
   int pid;			// process id
   int  fd;			// file descriptor
   int did;			// device id 
   VFS::Mount *mnt;	// mount
   int64 pos;			// file position
   void *data;			// data returned from FileSystem
};


class FileTable
{
  public: 
    FileTable();
   ~FileTable();

     int UniqueFD( int pid );

     int insertFile( struct file_info * win );
     int FileCount();
     struct file_info *getFile( int order );
     struct file_info *getFile( int pid, int fd );
    

       int removeFile( int pid, int fd );


  private:
     int Hash( int pid, int fd, unsigned int size );
     int Rehash( int newSize );

     int hashSize;
     struct file_info *hash_table; 
     int hashCount;
     struct file_info **hash_list;

  
};


#endif


