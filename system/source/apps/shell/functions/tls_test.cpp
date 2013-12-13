#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "tls_test.h"


struct function_info info_tls_test = 
{
   "spawns a few threads to test the TLS support",
   "tls_test [number]",
   tls_test_main
};

  char *tls_data = NULL;	// This is the application tls_location
  				// It's a different string per thread.

  int _tls_test( void *data )
   {
      if ( tls_data == NULL )
       {
         printf("%s\n","tls_data was NULL");
	 return -1;
       }

       printf("%s%s\n","tls_data: ", tls_data );

       free( tls_data );
       smk_set_tls_value( -1, NULL );	// Clean up the TLS data 
       tls_data = NULL;
       return 0;
   }


 int tls_test_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
   int i, tid;
   int a = atoi( argv[1] );
   
   if ( a > 250 ) a = 250;  // cap it because I make stupid mistakes sometimes.
  
   smk_set_tls_location( &tls_data );
  
   if ( a > 0 ) 
   {

     printf("%s%i\n","memory free before spawning: ", smk_info_mem_free() );
   
     for ( i = 0; i < a; i++ )
      {
        tid = smk_spawn_thread( _tls_test, 
	                    "_tls_test", 
			    NORMAL_PRIORITY,
			    (void*)NULL );

		// setup the TLS string for the thread.
	char *data = (char*)malloc(1024);
	sprintf(data, "%s%i", "I am thread ", i );

		// Tell the kernel where it is.
	smk_set_tls_value( tid, data );
			    
	smk_resume_thread( tid );		    
      }
      
      printf("%s%i\n","memory free after spawning: ", smk_info_mem_free() );
      printf("%s\n",  "sleeping 2 seconds" );
      smk_sleep(2);
      printf("%s%i\n","memory free after sleeping: ", smk_info_mem_free() );
   }
	
   return 0;
 }
 
