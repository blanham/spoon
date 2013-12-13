#ifndef _VFS_SERVER_FS_LIST_H
#define _VFS_SERVER_FS_LIST_H

#include "../io/DeviceTable.h"

class FileSystem;

typedef  FileSystem*(*create_fs)(VFS::Device*);

struct fs_info
{
   char *code;
   create_fs func;
};


FileSystem *new_fs( char *code, VFS::Device *dev );

#endif

