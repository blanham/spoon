#ifndef _DJFS_H
#define _DJFS_H

#include <types.h>

#define SECTOR_SIZE			512

#define TABLE_SECTOR		1
#define SUPERBLOCK_SECTOR	0

#define SUPER_SIZE	   		((int)(sizeof(struct djfs_superblock)))
#define FILE_SIZE	   		((int)(sizeof(struct djfs_file)))
#define TABLE_BLOCK_SIZE	((int)(sizeof(struct djfs_table_block)))
#define ENTRY_SIZE	   		((int)(sizeof(struct djfs_entry)))
#define TABLE_ENTRIES  		((int)((SECTOR_SIZE / sizeof(int)) - 1))


#define DJFS_MAGIC			0x444A4653
								// 'DJFS'


// -----------------------

#define DIRECTORY			1


// -----------------------


struct djfs_superblock
{
	uint32 magic;
	char label[256];
	int total_blocks;
	int free_blocks;
} __attribute__ ((packed));



struct djfs_file
{
	char filename[256];
	int table;
	int size;
	int location;
	unsigned int flags;
} __attribute__ ((packed));


struct djfs_entry
{
	char filename[256];
	int location;
} __attribute__ ((packed));

struct djfs_table_block
{
	int next;
	int list[ TABLE_ENTRIES  ];
} __attribute__ ((packed));


#endif



