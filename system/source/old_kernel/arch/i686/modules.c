#include <types.h>
#include <dmesg.h>
#include <macros.h>
#include <configure.h>
#include <conio.h>
#include <exec.h>
#include <elf.h>
#include <env.h>
#include <syscalls.h>


#include "memory.h"
#include "strings.h"
#include "multiboot.h"


static multiboot_info_t *mb = NULL;


void reserve_modules(  multiboot_info_t *mboot )
{
  uint32 i,j;
  module_t *mod;

  dmesg("reserving multiboot modules.\n");
  
  for (i = 0; i < mboot->mods_count; i++)
  {
     mod = (module_t*)( mboot->mods_addr + i * sizeof(module_t) );
	 dmesg( "%s%s\n","        module name: ", mod->string );

     for ( j = (mod->mod_start / PAGE_SIZE); 
           j <= (mod->mod_end / PAGE_SIZE); j++)
			memory_internalSet( j, 1 );
  }    

  mb = mboot;
}



// *****************************************


/**  This function gives the thread all capabilities. */

void godlike( struct process *proc )
{
  int i;
  struct thread *tr = proc->threads;
  
	  for ( i = 0; i < TOTAL_SYSCALLS; i++ )
	    tr->capabilities[i] = 0xFF;
}

/**	This one doesn't. */

void powerless( struct process *proc )
{
  int i;
  struct thread *tr = proc->threads;
  
	  for ( i = 0; i < TOTAL_SYSCALLS; i++ )
	    tr->capabilities[i] = 0x0;

}





/** build_modules() is a little function which runs through
 * all the modules passed to the kernel by GRUB and, if they
 * are ELF binaries and not environment variables, builds
 * the process image and prepares them for running.
 *
 */
int build_modules()
{
    int i,j,k;
    module_t *mod;
    ELF32_Ehdr *elf_header;
    struct process *proc;
	char *cmd_line;
	int pid;
	
    char name[OS_NAME_LENGTH];
	multiboot_info_t *multiboot_info = mb;

	    
	dmesg( "%s%i\n","number of modules loaded: ", multiboot_info->mods_count);

    for (i = 0; i < multiboot_info->mods_count; i++)
    {
      mod = (module_t*)( multiboot_info->mods_addr + i * sizeof(module_t) );
	  cmd_line = (char*)mod->string;
	  

	  dmesg( "%s%s\n", "module name: ", cmd_line );


	  // Check if it's an environment variable.
	  
		  pid = -1;
		  k = strlen( cmd_line ) - 5;  // - strlen(" env=");
		  for ( j = 0; j < k; j++ )
		  {
			if ( memcmp( cmd_line + j, " env=", 5 ) == 0 ) 
			{
				pid = 0x666;
				break;
			}
		  }
	
		  if ( pid == 0x666 ) 
		  {
			  dmesg( "%s\n", "    [skipping environment variable]" );
			  continue;	// skip environment variable.
		  }


	  // Check that's it's an ELF executable
      elf_header = (ELF32_Ehdr*) mod->mod_start;
      
      if ( (elf_header->e_ident[EI_MAG0] != 0x7f) ||
           (elf_header->e_ident[EI_MAG1] != 'E') ||
           (elf_header->e_ident[EI_MAG2] != 'L') ||
           (elf_header->e_ident[EI_MAG3] != 'F') ) 
	  		{
	  		   dmesg("%!%s%s\n","Unknown format for module: ", cmd_line );
	   		   dmesg("%!%s\n",  "Only ELF binaries are supported.");
	 		   while (1==1) {};
    		}
 

			  
	    // find the name
	      j = 0;
	      for ( k = 0; k < strlen( cmd_line ); k++ )
	      {
	        if ( cmd_line[k] == '/' ) j = k+1;
			if ( cmd_line[k] == ' ' ) break;
	      }

	      k = k - j;
	      if ( k >= OS_NAME_LENGTH ) k = OS_NAME_LENGTH - 1;
	    // got it.  j = start position, k = length

	      memcpy( name, (cmd_line + j), k );
	      name[k] = 0;
	     
		 	  pid = exec_memory_region( -1,
						      name, 
						      mod->mod_start, 
						      mod->mod_end,
						      cmd_line
					      );   

			  proc = checkout_process( pid, WRITER );
			  if ( proc == NULL ) return -1; /// \todo freak out
  
	          if ( i > 0 )  
		      {
	      	      powerless( proc );
	      	      godlike( proc );	/// \todo remove one day.
		          set_thread_state( proc->threads, THREAD_SUSPENDED );
		      }
		      else
		      {
	      	      godlike( proc );
	      	      proc->state = PROCESS_RUNNING;
		          set_thread_state( proc->threads, THREAD_RUNNING );
		      }
	   
			  commit_process( proc );

   
    }

	return 0;
}



/** These are the temporary variables/functions used to do the final environment
 * mapping. These are set up from build_environment_modules.
 */

static char *s_name     = NULL;
static char *s_hosts    = NULL;
static uintptr	s_start = 0;
static uintptr	s_end   = 0;


int build_envmods_callback( struct process *proc )
{
	int host = 0;

	// First determine if this process is a host;
	if ( s_hosts == NULL ) host = 1;		// All processes are hosts.
	else
	{
		int pos = 0;
		int i = 0;
		int len = strlen(s_hosts);

		while ( i <= len ) // Include the NULL character.
		{
			if ( (s_hosts[i] == ',') || (s_hosts[i] == 0) )
			{
				if ( (i - pos) > 1 )	// Accidental case of trailing comma or multiple commas in a row.
					if ( strncmp( s_hosts + pos, proc->name, (i - pos) ) == 0 )
					{
						host = 1;
						break;
					}

				pos = i + 1;
			}

			i++;
		}
	}

	// Now we should know...

	if ( host == 0 ) return 0;

	dmesg( "mapping %s into %s\n", s_name, proc->name );

	if ( set_environment( proc, s_name, (void*)s_start, (s_end - s_start + 1) ) != 0 )
	{
		dmesg( "%!%s\n", "Failed to map environment variable. This is bad." );
		while (1==1) {}
	}	

		
	return 0;
}



/** If a module has an environment name specified on the command
 * line (for example, env=data.zip), then this function will
 * map the module's data into all loaded processes as an
 * environment variable. 
 *
 * If an additional command line option of "env_target=[process
 * name[,process2,[..]]]" is around, then only the named
 * processes will have the module mapped into them as the 
 * environment variable.
 */

int build_environment_modules()
{
    module_t *mod;
    int i,j;
	char *env_name;
	char *env_hosts;
	char line[2048];

	int line_len;
	
	    
	dmesg( "Mapping environment variables.\n");

    for (i = 0; i < mb->mods_count; i++)
    {
      mod = (module_t*)( mb->mods_addr + i * sizeof(module_t) );
	  strcpy( line, (char*)mod->string );
	  
	  // Check if it's an environment variable.
	  
	  env_name = NULL;
	  line_len = strlen( line );
	  
		  for ( j = 0; j < (line_len - 5); j++ )
		  {
			if ( memcmp( line + j, " env=", 5 ) == 0 ) 
			{
				env_name = line + j + 5;
				break;
			}
		  }

	  if ( env_name == NULL )  continue;	// Not one.


	  // Find the target process hosts.
	  env_hosts = NULL;

		  for ( j = 0; j < (line_len - 11); j++ )
		  {
			if ( memcmp( line + j, " env_hosts=", 11 ) == 0 ) 
			{
				env_hosts = line + j + 11;
				break;
			}
		  }

	
	  // Let's clean up the line so that it's more useful.

	// ENV_NAME
		j = 0;
		while ( (env_name[j] != ' ') && (env_name[j] != 0) ) j++;
		while ( env_name[j] == ' ' ) 
		{
			env_name[j] = 0;
			j++;
		}
	
		if ( env_hosts != NULL )
		{
	  		j = 0;
			while ( (env_hosts[j] != ' ') && (env_hosts[j] != 0) ) j++;
			while ( env_hosts[j] == ' ' ) 
			{
				env_hosts[j] = 0;
				j++;
			}
		}
	
	// Geez, loo-eaze.
	
		  if ( strlen( env_name ) <= 0 ) continue; // Too small!	
		  if ( env_hosts != NULL ) 
				  if ( strlen( env_hosts ) <= 0 ) env_hosts = NULL;	// Too small. Just make it NULL.
		  
	// Okay.. we have our name and our hosts.

		  s_name  = env_name;
		  s_hosts = env_hosts;
		  s_start = mod->mod_start;
		  s_end   = mod->mod_end;
										  

		  foreach_process( build_envmods_callback, WRITER );
    }

	return 0;
}



/** This function releases all the memory that was reserved
 * for the modules. When the kernel first boots up, it marks
 * the memory where the modules are loaded as used so as to
 * not overrite the modules during normal initialization.
 *
 * This undoes it and allows the system to use that reserved
 * memory.
 */
void release_reserved_modules()
{
   int i, j;
   module_t *mod;

   dmesg("releasing memory reserved by modules.\n");
   
 // Release the memory they took up...
   for (i = 0; i < mb->mods_count; i++)
  {
     mod = (module_t*)( mb->mods_addr + i * sizeof(module_t) );
     
     for ( j = (mod->mod_start / PAGE_SIZE); 
     	   j <= (mod->mod_end / PAGE_SIZE); j++) 
	 {
		memory_internalSet( j, 0 );
	 }
  }    
  
}


/**  The multiboot boot loader will pass some information about the modules
 * it loaded to the kernel. This function will set each module up with a
 * task structure and everything else it needs.
 */


void arch_init_modules()
{
   build_modules();

   build_environment_modules();
   
   //release_reserved_modules();
}


