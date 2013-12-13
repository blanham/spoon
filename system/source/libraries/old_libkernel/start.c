#include <types.h>
#include <kernel.h>


/** \addtogroup START
 *
 * 
 * @{ 
 *
 */


extern int main(int argc, char** argv);


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
      void *ctors = (void*)(((uint32)&__ctor_list));

	   while  (  ((uint32)ctors) < ((uint32)&__ctor_end))
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
      void *dtors = (void*)(((uint32)&__dtor_list));
	     
	   while  (  ((uint32)dtors) < ((uint32)&__dtor_end))
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
      int bytes = ( ((uint32)&__bss_end) - ((uint32)&__bss_start) );
      if ( bytes <= 0 ) return;
      for ( i = 0; i < bytes; i++ ) bss[i] = 0;
  }


/** This is the function called from _start which is the environment
 * initialization code. It's responsibility is to zero the bss sections,
 * call the global constructors, set up the command line parameters,
 * etc, etc.
 *
 * This function then calls the standard main entry point with the
 * command line parameters.
 * 
 */
int spoon_start( uint32 eax, uint32 ebx, uint32 ecx, uint32 edx )
{
	int32 ans;
	int pcount;
	int i;
	char **parmlist;


	zero_bss();
	smk_init_console();
        call_ctors();

	pcount = 0;
	parmlist = NULL;

	parmlist = spoon_parse_parameters( &pcount, (char*)edx );

		ans = main( pcount , parmlist );	

	if ( parmlist != NULL ) 
	{
		for ( i = 0; i < pcount; i++)
		  smk_mem_free( parmlist[i] );
	
		smk_mem_free( parmlist );
	}

	call_dtors();

	return ans;
}


/** @} */

