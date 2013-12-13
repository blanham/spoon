#include <types.h>
#include "FileSystem.h"

#include "ext2fs_defs.h"

#ifndef _VFS_SERVER_FS_EXT2_H
#define _VFS_SERVER_FS_EXT2_H


FileSystem *create_ext2fs( VFS::Device *dev );


struct ext2_info
{
   struct ext2fs_inode inode;
};


class Ext2FS : public FileSystem
{
   public:
	Ext2FS( VFS::Device *dev );
	~Ext2FS();

   public:
	virtual int Init(); 
	virtual int Shutdown();

	virtual int Test();
   
   	virtual void *Open( const char *node ); 
	virtual int Read( int64 pos, void *buffer, int len, void *data );
	virtual int Write( int64 pos, void *buffer, int len, void *data );
	virtual void Close( void *data );

	virtual int Stat( struct stat *st, void *data );

	virtual int List( char *node, Message **list );

   private:
	struct ext2fs_superblock superblock;


   private:
	uint32 GetInodeByName(const char *name, struct ext2fs_inode *inode );
	uint32 GetInode(uint32 node_num, struct ext2fs_inode *inode );

	int32 ReadInode(struct ext2fs_inode inode, 
			void* buffer, 
			int64 location, 
			uint32 bytes);


	int32 GetStatByName(const char *name, struct stat *st);
	int32 GetStatByInode(uint32 inode_num, struct stat *st);


   private:
	uint32 block_size();
	uint32 read_block( uint32 block, void* data, uint32 num );
	uint32 get_group(  uint32 num, struct ext2fs_group *group);
	uint32 get_direntry(  struct ext2fs_inode inode, uint32 num, 
		      struct ext2fs_direntry *dir );
	uint32 translate_inode_pointer_to_block( 
                                         struct ext2fs_inode inode,
                                         uint32 pointer );
	
};


#endif

