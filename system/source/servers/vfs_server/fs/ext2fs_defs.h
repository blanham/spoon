#include <types.h>

#ifndef _VFS_SERVER_FS_EXT2FS_H
#define _VFS_SERVER_FS_EXT2FS_H


#define EXT2FS_MAGIC  0xef53
#define MAX_BLOCK_SIZE	65536

struct ext2fs_superblock 
{
            uint32 s_inodes_count;
            uint32 s_blocks_count;
            uint32 s_r_blocks_count;
            uint32 s_free_blocks_count;
            uint32 s_free_inodes_count;
            uint32 s_first_data_block;
            uint32 s_log_block_size;
            uint32 s_log_frag_size;
            uint32 s_blocks_per_group;
            uint32 s_frags_per_group;
            uint32 s_inodes_per_group;
            uint32 s_mtime;
            uint32 s_wtime;
            uint16 s_mnt_count;
            uint16 s_max_mnt_count;
            uint16 s_magic;
            uint16 s_state;
            uint16 s_errors;
            uint16 s_pad;
            uint32 s_lastcheck;
            uint32 s_checkinterval;
            uint32 s_creator_os;
            uint32 s_rev_level;
            uint16 s_def_resuid;
            uint16 s_def_resgid;
            uint32 s_reserved[235];
};


struct ext2fs_group 
{
        uint32 bg_block_bitmap;
        uint32 bg_inode_bitmap;
        uint32 bg_inode_table;
        uint16 bg_free_blocks_count;
        uint16 bg_free_inodes_count;
        uint16 bg_used_dirs_count;
        uint16 bg_pad;
        uint32 bg_reserved[3];
};


// ext2fs inode mode flags
#define MODE_MASK  0170000
#define MODE_FIFO  0010000
#define MODE_CDEV  0020000
#define MODE_DIR   0040000
#define MODE_BDEV  0060000
#define MODE_FILE  0100000
#define MODE_LINK  0120000
#define MODE_SOCK  0140000

// ext2fs extended flags
#define FLAG_MASK      0x007f
#define FLAG_SECURE    0x0001
#define FLAG_UNDELETE  0x0002
#define FLAG_COMPRESS  0x0004
#define FLAG_SYNC      0x0008
#define FLAG_NOCHANGE  0x0010
#define FLAG_APPONLY   0x0020
#define FLAG_NODUMP    0x0040

struct ext2fs_inode {
    uint16 i_mode;         // filemode
    uint16 i_uid;          // owner
    uint32 i_size;         // (bytes)
    uint32 i_atime;        // access
    uint32 i_ctime;        // "creation"
    uint32 i_mtime;        // modification
    uint32 i_dtime;        // deletion
    uint16 i_gid;          // user group
    uint16 i_links_count;  // hard links
    uint32 i_blocks;       // count
    uint32 i_flags;        // ext2fs
    uint32 i_reserved;     // [Linux] reserved
    uint32 i_block[15];    // 12 direct, indirect, double, triple
    uint32 i_version;      // NFS
    uint32 i_file_acl;     // Access Control List
    uint32 i_dir_acl;      // directory
    uint32 i_faddr;        // fragment
    uint8  i_frag;         // [Linux] number
    uint8  i_fsize;        // [Linux] (bytes)
    uint16 i_pad1;         // [Linux] padding
    uint32 i_reserved2[2]; // [Linux] reserved
};


// ext2fsDirectoryEntry

struct ext2fs_direntry 
{
    uint32 inode;
    uint16 rec_len;
    uint8  name_len;
    uint8  type;
    char name[1024];  // variable
};

// *******************************************

uint32 block_size( struct ext2fs_superblock sb );

uint32 read_block( int32 fd, struct ext2fs_superblock sb, uint32 block, void* data, uint32 num );

uint32 get_group( int32 fd, struct ext2fs_superblock sb,  uint32 num, struct ext2fs_group *group);

uint32 get_direntry( int32 fd,  struct ext2fs_superblock sb, 
		      struct ext2fs_inode inode, uint32 num, 
		      struct ext2fs_direntry *dir );

uint32 translate_inode_pointer_to_block( int32 fd, 
                                         struct ext2fs_superblock sb,
                                         struct ext2fs_inode inode,
                                         uint32 pointer );


// ****** USEFUL STUFF **********************



// ****** HIGHER LEVEL STUFF ****************


uint32 FindInodeByName( int32 fd, struct ext2fs_superblock sb, const char *name );
uint32 GetInode( int32 fd, struct ext2fs_superblock sb, uint32 node_num, struct ext2fs_inode *inode );

int32 ReadInode( int32 fd, struct ext2fs_superblock sb, struct ext2fs_inode inode, void* buffer, uint32 location, uint32 bytes);

int32 GetStatByName( int32 fd, struct ext2fs_superblock sb, const char *name, struct stat *st);
int32 GetStatByInode( int32 fd, struct ext2fs_superblock sb, uint32 inode_num, struct stat *st);





#endif



