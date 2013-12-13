#include <inttypes.h>
#include <assert.h>
#include <elf.h>
#include <dmesg.h>
#include <process.h>
#include <misc.h>

#include <paging.h>
#include <env.h>

#include <ualloc.h>



int exec_map_in( struct process *proc, 
		 			 uint32_t source, 
		 			 uint32_t destination, 
		  			 uint32_t size,
			   		 uint32_t copy )
{
	int pages = (size / PAGE_SIZE);
		pages += ((size % PAGE_SIZE) == 0) ? 0 : 1; 

		assert( (pages != 0) && (copy != 0) );
		

	if ( user_ensure( proc, destination, pages ) != 0 )
		   return -1;
   

   if (  page_copy_in( proc->map, destination, source, copy ) != copy )
	 return -1;


   return 0;
}




int exec_memory_region( int pid,
		 			    char * name, 
                  	    uint32_t start, 
					    uint32_t end,
		  				char *parameters )
{
  ELF32_Ehdr* header;
  ELF32_Phdr* ph;
  char *program_buffer;
  uint32_t i;
  int pd;
  int tid;
  struct process *proc = NULL;

  // First ensure that it's a valid ELF file.

  header = (ELF32_Ehdr*) start;
  program_buffer = (char*)start;
  
  if ( header->e_ident[ EI_MAG0 ] != 0x7f ) return -1; 
  if ( header->e_ident[ EI_MAG1 ] != 'E' ) return -1; 
  if ( header->e_ident[ EI_MAG2 ] != 'L' ) return -1; 
  if ( header->e_ident[ EI_MAG3 ] != 'F' ) return -1; 


  dmesg("(%i) executing image \"%s\" in [ %x : %x ] with \"%s\"\n",
				  pid, name,
				  start, end,
				  parameters );


  proc = new_process( name ); 
  if ( proc == NULL ) return -1;
  
  
	// ------

	  for ( i = 0; i < header->e_phnum; i++)
	    {	      
	     ph = (ELF32_Phdr*)( ((uint32_t)program_buffer) + 
	     			  header->e_phoff + 
				  ( header->e_phentsize * i) ) ;

		 // Pretty display printing
	      switch (ph->p_type)
	      {
			case 0x0: dmesg("%s"," null"); break;
	        case 0x1: dmesg("%s"," load"); break;
	        case 0x2: dmesg("%s"," dynamic"); break;
	        case 0x3: dmesg("%s"," interpreter"); break;
	        case 0x4: dmesg("%s"," note"); break;
	        case 0x5: dmesg("%s"," shlib"); break;
	        case 0x6: dmesg("%s"," phdr"); break;
			default:
				dmesg("%x", ph->p_type );
				break;
	      }
	      
		    dmesg("     Physical [%x:%x]  Virtual [%x:%x]\n",
			      start + ph->p_offset,
			      start + ph->p_offset + ph->p_filesz,
			      ph->p_vaddr,
			      ph->p_vaddr + ph->p_memsz );
			
			// here endeth the pretty display printing.
	

		 
	       if ((ph->p_type == 0x1) && (ph->p_memsz != 0))
	       {
			  if (  exec_map_in( proc, 
					   		start + ph->p_offset, 
							ph->p_vaddr, 
							ph->p_memsz, 
							ph->p_filesz ) != 0 ) 
			  {
				del_process( proc );
				return -1;
			  }
	       }
 
	    }




	pd = proc->pid;

  // Insert into the system.
	insert_process( pid, proc );

  // Okay.. the process is in the system. Get it back..

	proc = checkout_process( pd, WRITER );
	if ( proc == NULL ) 
	{
		/// \todo Clean up the process.
		return -1;		// what happened?!?
	}

  // ---------- ENVIRONMENT CLONING -----------------------------------

  		/// \todo Make safe
  		// While it's locked as a writer, let's clone the environment 
		// variables quickly and hope that pid is not locked! tsk.
  		if ( pid >= 0 )	// Only if we need to do it.
		{
			struct process *parent = checkout_process( pid, READER );
			assert( parent != NULL );
				// Holy smackerel. This is bad! An orphaned baby. why?!
				/// \todo Clean up the process.

			if ( clone_environment( proc, parent ) != 0 )
			{
				/// \todo Report it. Not really too critical. 
			}
			
			commit_process( parent );	// Back you go...
		}
  
  
   // Set up the environment variables
	if ( parameters != NULL )
		set_environment( proc, "CMD_LINE", parameters, strlen(parameters)+1 );


	// Spawn the main thread.
	  tid = new_thread( 0, proc, NULL, "main", header->e_entry, 
					    0 , 1, 2, 3, 4);


  commit_process( proc );
  return pd;
}


