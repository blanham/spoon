#include <alloc.h>
#include <env.h>
#include <process.h>
#include <misc.h>


///   \todo Evaluate whether or not I can switch to an htable here.



/** This is a structure used to store environment information for
 * the process.  
 */
struct environ_info
{
	char name[ENVIRONMENT_NAME_LENGTH];
	int size;
	void *data;
};



/** Increases the process' environment memory and adds the information to the
 * last position.
 *
 * \warning Currently loses all environment variables if system runs
 * out of memory, due to realloc retuning NULL on failure.
 * 
 * \param proc A W-LOCKED process or dead process, please.
 * \param name The environment name which will identify the data.
 * \param data The actual data to store.
 * \param size The number of bytes in data.
 * \return 0 on successful addition. 
 */ 
int	set_environment( struct process *proc, char *name, void *data, int size )
{
	struct environ_info *env;
   	int pos = proc->environment_count;
	int new_size = (pos + 1)  *  sizeof( struct environ_info );
	void *tmp;

	// Sanity check.
	if ( data == NULL ) return -1;
	if ( size <= 0 ) return -1;
	if ( name == NULL ) return -1;
	
	// Try and allocate memory for it first,
	tmp = malloc( size );
	if ( tmp == NULL ) return -1;
	
	// Resize the location.
	proc->environment = realloc( proc->environment, new_size );
	if ( proc->environment == NULL )
	{
		// realloc failed!
		proc->environment_count = 0;
		free( tmp );
		return -1;
	}
	
	// Copy the data in.
	memcpy( tmp, data, size ); 
	
	// Set the environment up.
	env = (struct environ_info *)proc->environment;

	strncpy( env[ pos ].name, name, ENVIRONMENT_NAME_LENGTH );  
	env[pos].data = tmp;
	env[pos].size = size;

	// Increase count.
	proc->environment_count += 1;
	return 0;
}


/** Returns the environment data which is identified with the given 
 * name. The size variable is given to limit the amount of information
 * returned in data.
 *
 *
 * \param proc A R-LOCKED process or dead process, please.
 * \param name The name of the environment variable to return.
 * \param data The buffer to copy the stored variable into.
 * \param size The largest amount of data to return in data.
 * \return the amount of bytes returned in data.
 */
int get_environment( struct process *proc, char *name, void *data, int size )
{
	struct environ_info *env = (struct environ_info *)proc->environment;
	int i;
	int new_size;

	for ( i = 0; i < proc->environment_count; i++ )
	{
		if ( strncmp( env[i].name, name, ENVIRONMENT_NAME_LENGTH ) == 0 ) 
		{
			// Okay, found it.
			new_size = env[i].size;
			if ( new_size > size ) new_size = size;

			memcpy( data, env[i].data, new_size );
			return new_size;
		}
	}

	return -1;
}



/** Returns the size of the environment variable with the given 
 * name,
 *
 * \param proc A R-LOCKED process or dead process, please.
 * \param name The name of the environment variable.
 * \param size The size of the data.
 * \return 0 on successful retrieval.
 */
int get_environment_size( struct process* proc, char *name, int *size )
{
	struct environ_info *env = (struct environ_info *)proc->environment;
	int i;

	for ( i = 0; i < proc->environment_count; i++ )
	{
		if ( strncmp( env[i].name, name, ENVIRONMENT_NAME_LENGTH ) == 0 ) 
		{
			*size = env[i].size;
			return 0;
		}
	}

	return -1;
}



/** Returns the name and size of the environment variable at the
 * given position in the environment list.
 *
 * 
 * \param proc A R-LOCKED process or dead process, please.
 * \param i the position of the environment variable.
 * \param name The name of the environment variable.
 * \param size The size of the data.
 * \return 0 on successful retrieval.
 */
int get_environment_information( struct process* proc, 
								     int i, char *name, int *size )
{
    struct environ_info *env = (struct environ_info *)proc->environment;
	
	if ( i < 0 ) return -1;
	if ( i >= proc->environment_count ) return -1;

	strncpy( name, env[i].name, ENVIRONMENT_NAME_LENGTH );
	*size = env[i].size;

	return 0;
}


/** Removes a particular environment variable from the process'
 * environment list and frees all associated data.
 *
 * \param proc A W-LOCKED process or dead process, please.
 * \param name The name of the environment variable to remove.
 * \return 0 on successful deletion.
 */
int remove_environment( struct process *proc, char *name )
{
	struct environ_info *env = (struct environ_info *)proc->environment;
	int found = 0;
	int i;
	int new_size;

	for ( i = 0; i < proc->environment_count; i++ )
	{
		if ( found == 0 )
		{
			if ( strncmp( env[i].name, name, ENVIRONMENT_NAME_LENGTH ) == 0 ) 
			{
				// Okay, found it.
				free( env[i].data );	// Free it.
				found = 1;				// Note it.
			}
		}
		else
		{
			// We are just correcting at the moment. Will never run on i==0.
			env[i - 1].data = env[i].data;
			env[i - 1].size = env[i].size;
			memcpy( env[i -1].name, env[i].name, ENVIRONMENT_NAME_LENGTH );
		}
		
	}

	if ( found == 0 ) return -1;	// No good.
	
	new_size = (proc->environment_count - 1)  *  sizeof( struct environ_info );

	// Safe for new_size == 0
	proc->environment = realloc( proc->environment, new_size );
	proc->environment_count -= 1;
	
	return 0;
}


/** Frees all environment information belonging to a process and
 * resets the process variables. So it's like it's fresh all
 * over.
 *
 * \param proc A W-LOCKED for writing or dead process please.
 * \return 0 on success.
 */
int release_environment( struct process *proc )
{
	int i;
	struct environ_info *env = (struct environ_info *)proc->environment;
		
	// Only if there's work.
	if ( env != NULL )
	{
		for ( i = 0; i < proc->environment_count; i++ )
			free( env[i].data );

		free( env );
	}
	
	proc->environment = NULL;
	proc->environment_count = 0;
	return 0;
}



/** This function takes the environment variables set up in the
 * source process and duplicates (copies) everything across to the
 * destination process.
 *
 * \warning This function first calls release_environment on the
 * destination process. Also, if an error occurs during the clone
 * then the destination process is left without any environment
 * variables.
 *
 * \param src The R-LOCKED source process to get environment variables from.
 * \param dest The W-LOCKED dest process to copy environment variables into.
 * \return 0 on success.
 */
int clone_environment( struct process *dest, struct process *src )
{
	int i;
	struct environ_info *env = (struct environ_info *)src->environment;

	if ( release_environment( dest ) != 0 ) return -1;
	
	if ( env == NULL ) return 0; 	// Already done. :)
		

	for ( i = 0; i < src->environment_count; i++ )
	{
		if ( set_environment( dest, env[i].name,
							   		env[i].data,
									env[i].size ) != 0 )
		{
			release_environment( dest );
			return -1;
		}	
	}

	
	return 0;
}




