#ifndef _VFS_H
#define _VFS_H




#define 	VFS_OPEN		10
#define 	VFS_READ		20
#define 	VFS_WRITE		30
#define 	VFS_CLOSE		40
#define 	VFS_SEEK		50
#define 	VFS_STAT		60
#define 	VFS_LIST		70
#define 	VFS_MKDIR		80
#define 	VFS_RMDIR		90
#define 	VFS_DELETE		100
#define 	VFS_EJECT		110
#define 	VFS_MOUNT		120
#define 	VFS_UNMOUNT		130
#define		VFS_REGISTER	140	
#define		VFS_DEREGISTER	150	
#define		VFS_UPDATE		160	


//


#define VFS_HEADER	\
			unsigned int operation;	\
			int rc;					\
			int err;				



/** Very, very basic structure which
 * contains the type of operation 
 * and return code.
 */
struct vfs_op
{
	VFS_HEADER
};


struct vfs_node_op
{
	VFS_HEADER
	int fd;
	unsigned int flags;
	unsigned int mode;
	unsigned int gid;
	unsigned int uid;
	char node;
};


struct vfs_transfer_op
{
	VFS_HEADER
	int fd;
	unsigned long long length;
	char data;
};

struct vfs_stat_op
{
	VFS_HEADER

	int dev_id;
	int inode;
	unsigned int mode;
	unsigned int nlink;
	
	unsigned int uid;
	unsigned int gid;

	int rdev_id;

	unsigned long long size;

	unsigned long long atime;
	unsigned long long mtime;
	unsigned long long ctime;
	
	unsigned long long block_size;
	unsigned long long blocks;
};


struct vfs_direntry
{
	unsigned long long size;
	unsigned int mode;
	unsigned int uid;
	unsigned int gid;
	unsigned long long mtime;
	char name;
};

struct vfs_dir_op
{
	VFS_HEADER

	unsigned long long entries;
	char data;
};



struct vfs_seek_op
{
	VFS_HEADER
	int fd;
	unsigned long long position;
};

struct vfs_mount_op
{
	VFS_HEADER

	char source[ 1024 ];
	char destination[ 1024 ];
	char file_system[ 64 ];
};


#endif




