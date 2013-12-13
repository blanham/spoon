#include <inttypes.h>
#include <assert.h>
#include <multiboot.h>
#include <misc.h>

#include <physmem.h>

#include <elf.h>

#include <exec.h>

#include <limits.h>

#include <smk.h>


#define MAXCMD			256
#define MAX_MODULES		16

static int l_moduleCount = 0;

static struct module_information
{
  char command_line[MAXCMD];
  uint32_t start;
  uint32_t finish;
} modules[MAX_MODULES];


int reserve_modules( multiboot_info_t *mboot )
{
	int i;
	int max = MAX_MODULES;
	module_t *mod;

	assert( l_moduleCount == 0 );

	if ( mboot->mods_count < MAX_MODULES )  max =  mboot->mods_count;

	for (i = 0; i < max; i++)
	{
		mod = (module_t*)( mboot->mods_addr + i * sizeof(module_t) );

		strncpy( modules[ l_moduleCount ].command_line, (char*)mod->string, MAXCMD );

		modules[ l_moduleCount ].start = mod->mod_start;
		modules[ l_moduleCount ].finish = mod->mod_end;


		internal_reserve( mod->mod_start, mod->mod_end, 1 );

		l_moduleCount += 1;
	}    
	
	dmesg("There are %i modules loaded.", l_moduleCount );
	
	return 0;
}







int build_module( int num )
{
    int j,k;
    ELF32_Ehdr *elf_header;
    char name[NAME_LENGTH];
	struct module_information *module = &(modules[num]);

	    


	    // find the name
	      j = 0;
	      for ( k = 0; k < strlen( module->command_line ); k++ )
	      {
	        if ( module->command_line[k] == '/' ) j = k+1;
			if ( module->command_line[k] == ' ' ) break;
	      }

	      k = k - j;
	      if ( k >= NAME_LENGTH ) k = NAME_LENGTH - 1;
	    // got it.  j = start position, k = length

	      memcpy( name, (module->command_line + j), k );
	      name[k] = 0;


	  dmesg("Module name: %s\n", name );
	  dmesg("command line: %s\n", module->command_line );
	  
	  // Check that's it's an ELF executable
      elf_header = (ELF32_Ehdr*) module->start;
      
      if ( (elf_header->e_ident[EI_MAG0] != 0x7f) ||
           (elf_header->e_ident[EI_MAG1] != 'E') ||
           (elf_header->e_ident[EI_MAG2] != 'L') ||
           (elf_header->e_ident[EI_MAG3] != 'F') ) 
	  		{
	  		   dmesg("%!%s\n","Unknown format for module."  );
	   		   dmesg("%!%s\n",  "Only ELF binaries are supported.");
	 		   while (1==1) {};
    		}


	exec_memory_region( smk_process->pid,
							name,
							module->start,
							module->finish,
							module->command_line );
	  
	return 0;
}







int init_modules()
{
	int i = 0;
	
	for ( i = 0;  i < l_moduleCount; i++ )
	{
		build_module( i );
	}



	// Release the memory that was used during the reserve.
	for ( i = 0;  i < l_moduleCount; i++ )
		internal_reserve( modules[i].start, modules[i].finish, 0 );

	return 0;
}


#include <scheduler.h>

int start_first_module()
{
	int ans = 0;
	struct process *proc = checkout_process( 2, READER );
	if ( proc == NULL ) return -1;
	
	struct thread *tr = get_thread( proc, 1 ); 
	if ( tr == NULL )
	{
		commit_process( proc );
		return -1;
	}
	
		ans = queue( tr );


	commit_process( proc );

	return ans;
}




