#ifndef _LIBKERNEL_SHMEM_H
#define _LIBKERNEL_SHMEM_H


#ifdef __cplusplus
extern "C" {
#endif

#define SHMEM_RDWR			1		//< region writable
#define SHMEM_IMMORTAL		2		//< persists after owner's death
#define SHMEM_PROMISCUOUS	4		//< allows anyone to request.


#define SHMEM_ORPHAN		32		//< You can't set this but it is returned by get_shmem_info if the owner pid is dead.
		

/** Create a shared memory region with the given name, amount
 * of pages and with the flags. */
int smk_create_shmem( const char name[ OS_NAME_LENGTH ], 
						int pages, 
						unsigned int flags );

/** Grant access to the shared memory to pid with the flags as permission */
int smk_grant_shmem( int id, int pid, unsigned int flags );

/** Revoke access.  The other process is unaware that it's using "fake" memory. */
int smk_revoke_shmem( int id, int pid );


/** Request the shared memory region. The only information you need to 
 * provide is the id of the shared memory you need.  You also need to 
 * provide valid pointers for the return information. That's about it.
 */
int smk_request_shmem( int id, void **location, int *pages, unsigned int *flags );

/** Quite simply, release the shared memory with id ID */
int smk_release_shmem( int id );

/** Delete the shared memory region with the given ID. All other
 * processes using the region have fake memory regions put in
 * place.
 */
int smk_delete_shmem( int id );

/** Return the pid and the ID of the shared memory region with given
 * name.
 */
int smk_find_shmem( const char name[ OS_NAME_LENGTH ], int *pid );


/** Gets the information about a particular shared memory region. 
 *
 * \return 0 if successful.
 * */
int smk_get_shmem_info( int id, char name[ OS_NAME_LENGTH ],
							int *pid, int *pages, unsigned int *flags );


#ifdef __cplusplus
}
#endif

#endif

