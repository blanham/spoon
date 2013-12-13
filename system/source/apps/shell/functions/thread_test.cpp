#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "thread_test.h"


struct function_info info_thread_test = 
{
   "spawns a lot of threads",
   "thread_test [number]",
   thread_test_main
};

  int _thread_test( void *data )
   {
		 smk_sleep(10);
		 return 0;
   }


 int thread_test_main( int argc, char **argv )
 {
   if ( argc != 2 ) return -1;
   
   int i, tid;
   int a = atoi( argv[1] );
   
   if ( a > 250 ) a = 250;  // cap it because I make stupid mistakes sometimes.
   
   if ( a > 0 ) 
   {

     printf("%s%i\n","memory free before spawning: ", smk_info_mem_free() );
   
     for ( i = 0; i < a; i++ )
      {
        tid = smk_spawn_thread( _thread_test, 
	    	                "_thread_test", 
						    NORMAL_PRIORITY,
						    (void*)NULL );
		smk_resume_thread( tid );		    
      }
      
      printf("%s%i\n","memory free after spawning: ", smk_info_mem_free() );
      printf("%s\n",  "sleeping 10 seconds" );
      smk_sleep(20);
      printf("%s%i\n","memory free after sleeping: ", smk_info_mem_free() );
   }
	
   return 0;
 }
 
