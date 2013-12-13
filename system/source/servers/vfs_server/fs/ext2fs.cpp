#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <spoon/storage/StorageDefs.h>
#include "ext2fs_defs.h"
#include "ext2fs.h"



FileSystem *create_ext2fs( VFS::Device *dev )
{
  return new Ext2FS( dev );
}

// **************************************************



Ext2FS::Ext2FS( VFS::Device *dev )
: FileSystem( "ext2fs", "The second extended file system", dev)
{
}

Ext2FS::~Ext2FS()
{
}

// ***********************************************

int Ext2FS::Init()
{
  int i;

  i = dev_read( 1024,  &superblock, sizeof( struct ext2fs_superblock ) );

  if ( i != sizeof( struct ext2fs_superblock ) ) return -1;
  if ( superblock.s_magic != EXT2FS_MAGIC ) return -1;
  return 0;
}

int Ext2FS::Shutdown()
{
  return 0;
}


// **********************************************

void *Ext2FS::Open( const char *node )
{
  struct ext2fs_inode inode;
  uint32 inum;
  
  inum = GetInodeByName( node, &inode );
  if ( inum == 0 ) return NULL;
 
  struct ext2_info *info = (struct ext2_info*)
  				malloc( sizeof( struct ext2_info ) );

  info->inode = inode;
  return info;
}


int Ext2FS::Read( int64 pos, void *buffer, int len, void* data )
{
	struct ext2_info *info = (struct ext2_info*)data;

	return ReadInode( info->inode, buffer, pos, len );
}

int Ext2FS::Write( int64 pos, void *buffer, int len, void *data )
{
	return -1;
}

int Ext2FS::Stat( struct stat *st, void *data )
{
	struct ext2_info *info = (struct ext2_info*)data;

	struct ext2fs_inode *inode = &(info->inode);

	   st->st_mode 		= inode->i_mode;
	   st->st_dev 		= GetDevice()->getDeviceID();
	   st->st_size 		= inode->i_size;
		
	return 0;
}


int Ext2FS::List( char *node, Message **list )
{
   struct ext2fs_inode inode;
   struct ext2fs_direntry dir;

   *list = NULL;

   uint32 inum = GetInodeByName( node, &inode );
   if ( inum == 0 ) return -1;

   Message *msg = new Message(OK);

   	int number = 0;
	while ( get_direntry( inode, number++, &dir ) == 0 )
	{
		dir.name[ dir.name_len ] = 0;
		msg->AddString("entry", dir.name );
		if ( number > 1000 )  break;
	}
	*list = msg;

  return 0;
}

// -----------------------------------------------

int Ext2FS::Test( )
{
	struct ext2fs_superblock sb;
	int i;

        i = dev_read( 1024, &sb, sizeof( struct ext2fs_superblock ) );

	if ( i != sizeof( struct ext2fs_superblock ) ) return FS_UNSUPPORTED;
	if ( sb.s_magic != EXT2FS_MAGIC ) return FS_UNSUPPORTED;
	return FS_SUPPORTED;
}


void Ext2FS::Close( void *data )
{
	free( data );
}

// **********************************************************

uint32 Ext2FS::block_size()
{
        if ( superblock.s_log_block_size == 0 ) return 1024;
        if ( superblock.s_log_block_size == 1 ) return 2048;
        if ( superblock.s_log_block_size == 2 ) return 4096;
        if ( superblock.s_log_block_size == 3 ) return 8192;
        if ( superblock.s_log_block_size == 4 ) return 16384;
        if ( superblock.s_log_block_size == 5 ) return 32768;
        if ( superblock.s_log_block_size == 6 ) return 65536;

	return 0;
}


// Blocks start at 0

uint32 Ext2FS::read_block( uint32 block, void* data, uint32 num )
{
	uint32 sector;
	uint32 count;

	sector = (block * block_size());
	 count = (num * block_size());

	 count = dev_read( sector,  data, count  );

	return count;
}


// Groups start at 0

uint32 Ext2FS::get_group(  uint32 num, struct ext2fs_group *group)
{
	uint32 gr_block;  
	uint32 gr_offset;
	uint32 groups_per_block;
	uint32 block;
	struct ext2fs_group *buffer;


	groups_per_block = block_size() / sizeof( struct ext2fs_group );
	buffer = (struct ext2fs_group*)malloc(  block_size() );

	gr_block  = num / groups_per_block ;
	gr_offset = num % groups_per_block ;

	//  we add 1 to it to get passed the superblock.

	block = gr_block;
	block = block + superblock.s_first_data_block + 1;
	
	read_block( block, buffer, 1 );

	*group = buffer[gr_offset];

	free( buffer );

	return 0;
}


// These start at 0 too

uint32 Ext2FS::get_direntry(  struct ext2fs_inode inode, uint32 num, 
		      struct ext2fs_direntry *dir )
{
	char *buffer;
	struct ext2fs_direntry *d;
	uint32 i;
	uint32 offset;
        uint32 temp;
	uint32 block;

	buffer = (char*)malloc( block_size() );
	
	block = 0;
	offset = 0;

	read_block( translate_inode_pointer_to_block(inode,block) , 
		    buffer, 
		    1 ); 

	for ( i = 0; i <= num; i++ )
	{
           d = (struct ext2fs_direntry*)(buffer + offset);

	   if ( d->rec_len < 0xC ) { free(buffer); return 1; }
	   if ( d->rec_len < (d->name_len & 0xFF) ) { free(buffer); return 1; }
	   if ( d->rec_len > block_size() ) { free(buffer); return 1; }

	   if ( (d->name_len & 0xFF) == 0  ) { free(buffer); return 1; }
	   if ( (d->inode >= superblock.s_inodes_count ) ) { free(buffer); return 1; }

	   offset = offset + d->rec_len;


           if ( (block * block_size() + offset) > inode.i_size ) { free(buffer); return 0; }

 	   if ( i == num) break;
	   
	   if ( offset == block_size() )
	   {
	       offset = 0;
	       block += 1;
              temp = translate_inode_pointer_to_block( inode, block );
	      read_block( temp, buffer, 1); 
	   }

	   if ( offset > block_size() )
	   {
		   printf("%s\n","problem: offset > block_size()");
		   free(buffer);
		   return 0;
	   }
   
	}

	*dir = *d;
	free(buffer);
	return 0;
}

uint32 Ext2FS::translate_inode_pointer_to_block( 
                                         struct ext2fs_inode inode,
                                         uint32 pointer )
{
   uint32 trans_offset;
   uint32 pointers_per_block;
   uint32 i;

   uint32 *table;
   
   table = (uint32*)malloc( 4 *  MAX_BLOCK_SIZE );

   if ( pointer < 12 ) { free(table); return inode.i_block[ pointer ]; }

   pointers_per_block = block_size() / sizeof(uint32);

   if ( pointer < 12 + pointers_per_block )
   {
       trans_offset = (pointer - 12) % pointers_per_block;
       read_block( inode.i_block[12], table, 1 );
       i = table[trans_offset];
       free( table );
       return i;
   }

   if ( pointer < 12 + pointers_per_block*pointers_per_block 
                     + pointers_per_block )
   {
     trans_offset = (pointer - pointers_per_block - 12) / (pointers_per_block * pointers_per_block);
     read_block( inode.i_block[13], table, 1 );
     read_block( table[trans_offset], table, 1 );
     trans_offset = ( pointer - pointers_per_block - 12 ) % ( pointers_per_block * pointers_per_block ); 
     i = table[trans_offset];
     free(table);
     return i;
   }

   if ( pointer < 12 + pointers_per_block*pointers_per_block*pointers_per_block  + pointers_per_block*pointers_per_block + pointers_per_block)
   {
     trans_offset = (pointer - pointers_per_block * pointers_per_block - pointers_per_block - 12) / (pointers_per_block * pointers_per_block * pointers_per_block);
     read_block( inode.i_block[14], table, 1 );
     read_block( table[trans_offset], table, 1 );

     trans_offset = ( pointer - pointers_per_block * pointers_per_block - pointers_per_block - 12) % ( pointers_per_block * pointers_per_block * pointers_per_block ); 
     read_block( table[trans_offset], table, 1 );

     // -------- HAPPY UP TO HERE

     trans_offset = ( pointer - pointers_per_block * pointers_per_block - 12 ) %  ( pointers_per_block  ); 
     i = table[trans_offset];
     free(table);
     return i;
   }




   printf("%s%i\n","FILE SIZE IS TOO LARGE: block ",pointer );
   exit(0);
   return 0;
}

// ****************************************************************************


uint32 Ext2FS::GetInodeByName(const char *name, struct ext2fs_inode *inode )
{
	struct ext2fs_inode tmp_node;
	struct ext2fs_direntry direntry;
	char temp_name[1024], compare_name[1024];
	uint32 temp_pos, name_pos;
	uint32 i;
	uint32 found,final;

        final = 0;

        if ( strcmp(name,"") == 0 ) // root inode
	{
		GetInode( 2, inode );
		return 2;
	}

	GetInode( 2, &tmp_node );

//	printf("%s","");
//	printf("%s\n","god");

	temp_pos = 0;
	name_pos = 0;
        if ( name[0] == '/' ) name_pos++;

	while (name_pos < strlen(name) - 1)
	{
           temp_pos = name_pos;
           while (name[name_pos] != '/')
           {
         	name_pos++;
         	if (name_pos == strlen(name) - 1) 
		{
	  	  name_pos++;
		  final = 1;
		  break;
		}
           }

           memcpy( &  temp_name[0], &(name[temp_pos]), name_pos - temp_pos );
           temp_name[ name_pos - temp_pos  ] = 0;

	   printf("%s%s\n","temp name = ", temp_name );

	   i = 0;
	   found = 0;
	   while (  get_direntry( tmp_node, i++, &direntry ) == 0 )
	   {
	     memcpy( compare_name, direntry.name, direntry.name_len & 0xFF );
	     compare_name[ direntry.name_len & 0xFF ] = 0;
	     printf("%s%i\n","name len = ", direntry.name_len & 0xFF );
	     printf("%s%s\n","direntry name = ", compare_name );
	     if ( strcmp( compare_name, temp_name ) == 0 )
	     {
	        printf("%s%i\n","FOUND FOUND FOUND == ",direntry.inode);
			if ( final == 1 ) 
			{
				GetInode( direntry.inode , inode );
				return direntry.inode;
			}
		GetInode( direntry.inode , &tmp_node );
		found = 1;
		break;
	     }
	   }

           if ( found == 0 )  break;
           name_pos++;
	}

   return 0;	
}


// Inodes start at 1
//
//
//   This code translates it onto the 0-end format

uint32 Ext2FS::GetInode( uint32 node_num, 
		 struct ext2fs_inode *inode )
{
	uint32 group;
	uint32 left_over_inodes;
	
	uint32 offset;
        uint32 block;
	
	uint32 inodes_per_block;
	struct ext2fs_group gr;
	struct ext2fs_inode *buffer;

	
	buffer = (struct ext2fs_inode*)malloc( block_size()  );

// ------- !!!!!!!!!

//printf("%i",i++);

	group = (node_num - 1) / superblock.s_inodes_per_group;
	left_over_inodes = (node_num - 1) % superblock.s_inodes_per_group;
	
	inodes_per_block = block_size() / sizeof( struct ext2fs_inode );
	
//printf("%i",i++);
	get_group(  group, &gr);
//printf("%i",i++);

        block = gr.bg_inode_table + left_over_inodes / inodes_per_block;
	offset = left_over_inodes % inodes_per_block ;
	
//printf("%i",i++);
        read_block( block, buffer, 1 );
//printf("%i\n",i++);

	*inode = buffer[offset];

	free(buffer);
	return 0;
	
}



int32 Ext2FS::ReadInode(  struct ext2fs_inode inode, 
			  void* buffer, 
			  int64 locatio, 
			  uint32 bytes)
{
    uint32 current_block;
    uint32 current_offset;
    uint32 counter;
    uint32 temp_block;
    int location = (int)locatio;
    char *data;

    data = (char*)malloc( 8192 );

    current_block = location / block_size();
    current_offset = location % block_size();
	
    read_block( translate_inode_pointer_to_block( inode, current_block), data, 1 );
    for ( counter = 0; counter < bytes; counter++)
    {
	    if ( current_offset >= block_size() )
	    {
		    current_offset = 0;
		    current_block++;

                    temp_block = translate_inode_pointer_to_block( inode, 
								   current_block );
		    read_block( temp_block, data, 1 );
	    }
	    ((char*)buffer)[counter] = data[current_offset];
	    current_offset++;
    }
    
   free( data );
   return bytes; 
}


int32 Ext2FS::GetStatByName( const char *name, struct stat *st)
{
  struct ext2fs_inode inode;
  uint32 inode_num = GetInodeByName( name, &inode );
  return GetStatByInode( inode_num, st );
}


int32 Ext2FS::GetStatByInode( uint32 inode_num, struct stat *st)
{
/*
   struct ext2fs_inode inode;
   GetInode( inode_num, &inode );
 
   st->st_ino 		= inode_num;
   st->st_mode 		= inode.i_mode;
   st->st_nlink 	= inode.i_mode;
   st->st_uid 		= inode.i_uid;
   st->st_gid 		= inode.i_gid;
   st->st_rdev 		= inode.i_mode;
   st->st_size 		= inode.i_size;
   st->st_blksize	= block_size();
   st->st_blocks	= inode.i_blocks;
   st->st_atime 	= inode.i_atime;
   st->st_mtime 	= inode.i_mtime;
   st->st_ctime 	= inode.i_ctime;
*/
   return 0;
}



