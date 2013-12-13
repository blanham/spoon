#ifndef _KERNEL_SHMEM_H
#define _KERNEL_SHMEM_H

#include <types.h>
#include <lengths.h>
#include <process.h>
#include <rwlock.h>


#define SHMEM_RDWR			1		//< region writable
#define SHMEM_IMMORTAL		2		//< persists after owner's death
#define SHMEM_PROMISCUOUS	4		//< allows anyone to request.

#define SHMEM_ORPHAN		32		//< Indicates whether the owner is dead.


#define SHMEM_MASK			(1|2|4)	//< Mask for all the valid SHMEM flags that userland can set.

/** A structure detailing which application the shared memory region
 * is mapped into.
 */
struct shmem_info
{
	int pid;						//< PID that is using this shmem
	uintptr virtual_location;		//< Virtual location of physical location.
	int virtual_pages;				//< Number of virtual pages mapped in.
	unsigned int flags;				//< Read only, read-write, etc

	int usage;						//< Number of times it has been requested.

	void *tmp;						//< Small tmp variable for easy deletion.
	void *tmp2;						//< Small tmp variable for easy deletion.

	struct shmem_info *next;		//< linked list information.
	struct shmem_info *prev;		//< linked list information.
};


/** The master structure detailing the exact shared memory information */
struct shmem
{
	int id;								//< Shared memory ID.
	char name[ OS_NAME_LENGTH ];		//< Shared memory name.

	struct rwlock *lock;				//< The reader-writer lock.
	
	int pid;							//< Shared memory owner.
	unsigned int flags;					//< Flags detailing the shared memory. 

	uintptr physical_location;			//< Physical location.
	int     physical_pages;				//< Number of pages of physical location.
	
	struct shmem_info *list;			//< The list of users.	
};




int init_shmem();
int shutdown_shmem();


int create_shmem( const char name[ OS_NAME_LENGTH ], int pid, int pages, unsigned int flags );
				
int grant_shmem( int owner, int id, int pid, unsigned int flags );
int revoke_shmem( int owner, struct process *proc, int id );
				
int request_shmem( struct process *proc, int id, void **location, int *pages, unsigned int *flags );
int release_shmem( struct process *proc, int id );

int delete_shmem( int owner, int id );

int clean_shmem( int pid );


int find_shmem( const char name[ OS_NAME_LENGTH ], int *pid );

int get_shmem( int i, char name[ OS_NAME_LENGTH], int *pid, int *pages, unsigned int *flags );


#endif









