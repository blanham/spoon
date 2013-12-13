#include <process.h>
#include <strings.h>
#include <threads.h>
#include <configure.h>
#include <paging.h>
#include <memory.h>
#include <dmesg.h>
#include <elf.h>
#include <memory.h>
#include <process.h>
#include <console.h>
#include <ualloc.h>
#include <env.h>
#include <smk.h>

/* **************************** ELF based loading only ********************************************  */

int exec_map_in( vmmap *map, 
		 			 uintptr source, 
		 			 uintptr destination, 
		  			 uintptr size,
			   		 uintptr copy )
{
   if ( page_ensure( map, destination, (size / PAGE_SIZE) + 1,
						USER_PAGE | READ_WRITE ) != 0 )
		   return -1;
   

   if (  page_copy_in( map, destination, source, copy ) != copy )
	 return -1;


   return 0;
}





int exec_memory_region( int pid,
		 			    char * name, 
                  	    uint32 start, 
					    uint32 end,
		  				char *parameters )
{
  ELF32_Ehdr* header;
  ELF32_Phdr* ph;
  char *program_buffer;
  uint32 params;
  uint32 i;
  int pd;
  int tid;

  struct process*	proc;
  struct process*	parent;


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
				  
  
	// ------

  
  proc = new_process( name );
  if ( proc == NULL ) return -1;



  

	  for ( i = 0; i < header->e_phnum; i++)
	    {	      
	     ph = (ELF32_Phdr*)( ((uint32)program_buffer) + 
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
	

		 
	       if (ph->p_type == 0x1)
	       {
			  if (  exec_map_in( proc->map, 
					   		start + ph->p_offset, 
							ph->p_vaddr, 
							ph->p_memsz, 
							ph->p_filesz ) != 0 ) 
			  {
				delete_process( proc );
				return -1;
			  }
	       }
 
	    }


   // ---------------------------------

	  params = 0;

	  if ( parameters != NULL )
	  {
	      params = user_alloc( proc, strlen(parameters) / PAGE_SIZE + 1);
	      page_copy_in( proc->map, params, (uintptr)parameters, strlen(parameters) + 1 );
	  }


  pd = proc->pid;

  

  // Insert into the system.
  
  insert_process( pid, proc );

  // Okay.. the process is in the system. Get it back..

  proc = checkout_process( pd, WRITER );
  if ( proc == NULL ) 
	{
		/// \todo Clean up the process.
		  return -1;		// what?!?
	}

  // ---------- ENVIRONMENT CLONING -----------------------------------

  		// While it's locked as a writer, let's clone the environment 
		// variables quickly and hope that pid is not locked! tsk.
  		if ( pid >= 0 )	// Only if we need to do it.
		{
  			parent = checkout_process( pid, READER );
			if ( parent == NULL ) 
			{
				commit_process( proc );
				// Holy smackerel. This is bad! An orphaned baby. why?!
				/// \todo Clean up the process.
				return -1;
			}

			if ( clone_environment( proc, parent ) != 0 )
			{
				/// \todo Report it. Not really too critical. 
				return -1;
			}
			
			commit_process( parent );	// Back you go...
		}
  
  
	// ----------------------------------------------------------------
  
  		// Spawn the main thread.
	  tid = new_thread( 0,
					    proc, 
					    NULL, 
					    "main", 
					    header->e_entry, 
					    0 ,
					    1,
					    2,
					    3,
					    params
					   );


	  if ( proc->parent == NULL ) attach_console( proc, 0 );
	  else attach_console( proc, proc->parent->console );	  

  commit_process( proc );
  return pd;
}





