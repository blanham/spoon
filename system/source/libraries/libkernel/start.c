#include <kernel.h>


/** \addtogroup START
 *
 * 
 * @{ 
 *
 */



char** spoon_parse_parameters( int *argc, char* params)
{
	char **parmlist;
	char *tmp;
	int pcount;
	int state;
	int i,j;
	int length;
	int start;


	if ( params == NULL ) 
	{
		*argc = 0;
		return NULL;
	}



	length = kstrlen( params );	// this is going to 
	 				// change in this routine,
					// so we save it first.

	 parmlist = (char**)smk_mem_alloc( 1 );

	 state = 0;
	 start = -1;
	 pcount = 0;
	
	 for ( i = 0; i < length; i++ )
	 {
	   if ( (params[i] != ' ') && (state == 0))	// new param
	   {
	        start = i;
		state = 1;
	   }

	   if (  (params[i] == ' ') && (state == 1) )
	   {
	        state = 0;
		
		tmp = (char*) smk_mem_alloc( 1 ); 
		kstrcpy( tmp, (char*)(params + start) );
		for ( j = 0; j < kstrlen( tmp ); j++ )
		   if ( tmp[j] == ' ' ) 
		   {
		     tmp[j] = 0;
		     break;
		   }

		parmlist[ pcount++ ] = tmp;
		start = -1;
	   }

	 }

	// Something left over...
	 if ( start >= 0 )
	 {
  	   tmp = (char*) smk_mem_alloc( 1 ); 
	   kstrcpy( tmp, params + start );
  	   for ( j = 0; j < kstrlen( tmp ); j++ )
	    if ( tmp[j] == ' ' ) 
	    {
	      tmp[j] = 0;
	      break;
	    }
	    
	   parmlist[ pcount++ ] = tmp; 
	 }
	// ------------------------
	

	*argc = pcount;
	return parmlist;
}


extern void *__bss_start;
extern void *__bss_end;
extern void *__ctor_list;
extern void *__ctor_end;
extern void *__dtor_list;
extern void *__dtor_end;

typedef void (*ctor)();


  void call_ctors()
  {
     ctor construct;
      
     	// Please keep __ctor_list aligned in the linker script
      void *ctors = (void*)(((unsigned int)&__ctor_list));

	   while  (  ((unsigned int)ctors) < ((unsigned int)&__ctor_end))
	   {
	     void **indirect = (void**)ctors;
	     construct = (ctor)(*indirect);
	     construct();
	     ctors = (void*)(ctors + sizeof(void*));
	   }

  }

  void call_dtors()
  {
     ctor destruct;

     	// Please keep __dtor_list aligned in the linker script
      void *dtors = (void*)(((unsigned int)&__dtor_list));
	     
	   while  (  ((unsigned int)dtors) < ((unsigned int)&__dtor_end))
	   {
	     void **indirect = (void**)dtors;
	     destruct = (ctor)(*indirect);
	     destruct();	
	     dtors = (void*)(dtors + sizeof(void*));
	   }

  }


  void zero_bss()
  {
      int i;
      char *bss = (char*)(&__bss_start);
      int bytes = ( ((unsigned int)&__bss_end) - ((unsigned int)&__bss_start) );
      if ( bytes <= 0 ) return;
      for ( i = 0; i < bytes; i++ ) bss[i] = 0;
  }




/** This is the external declaration for the standard C main function */
extern int main(int argc, char** argv);


/** This is the function called from _start which is the environment
 * initialization code. It's responsibility is to zero the bss sections,
 * call the global constructors, set up the command line parameters,
 * etc, etc.
 *
 * The command line parameters are set up in the kernel as an
 * environment variable with the name "CMD_LINE"
 * 
 * This function then calls the standard main entry point with the
 * command line parameters.
 * 
 */
int spoon_start( unsigned int eax, unsigned int ebx, unsigned int ecx, 
				 unsigned int edx )
{
	int i, ans, pcount = 0;
	char **parmlist = NULL;
	void *command_line = NULL;


	zero_bss();			// zero BSS
    call_ctors();		// Call constructors


	// Allocate some memory to hold the command line parameters.
	i = smk_getenv_size( "CMD_LINE" );
	if ( i > 0 )
	{
		ans = i / 4096;	/// \todo 4096 should be defined somewhere.
		ans += ((i % 4096)==0) ? 0 : 1;

		command_line = smk_mem_alloc( ans );

		if ( command_line != NULL )
			if ( smk_getenv( "CMD_LINE", command_line, ans * 4096 ) <= 0)
			{
				smk_mem_free( command_line );
				command_line = NULL;
			}

	}

	
	// Parse the command line parameters
	parmlist = spoon_parse_parameters( &pcount, command_line );

	// Call the application's main function
		ans = main( pcount , parmlist );	


	// Free up any resources we allocated.
		
	if ( parmlist != NULL ) 
	{
		for ( i = 0; i < pcount; i++)
		  smk_mem_free( parmlist[i] );
	
		smk_mem_free( parmlist );
	}

	// Free the environment variables we had set up.
	if ( command_line != NULL ) smk_mem_free( command_line );

	call_dtors();	// Call destructors
	return ans;
}


/** @} */

