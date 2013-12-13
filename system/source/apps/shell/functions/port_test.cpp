#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "port_test.h"


struct function_info info_port_test = 
{
   "port_test",
   "port_test creates and destroys IPC ports.",
   port_test_main
};


 int port_test_main( int argc, char **argv )
 {
   int port;

      
     if ( smk_create_port( 600, smk_gettid() ) == 0 )
      {
        printf("%s\n","Successfully opened port 600" );
		smk_release_port( 600 );
      }

     if ( smk_create_port( 601, smk_gettid() ) == 0 )
      {
        printf("%s\n","Successfully opened port 601" );
		smk_release_port( 601 );
      }

     if ( smk_create_port( 600, smk_gettid() ) == 0 )
     {
      if ( smk_create_port( 600, smk_gettid() ) == 0 )
      {
        printf("%s\n","Freakishly able to open 600 twice." );
      } 
      smk_release_port( 600 );
    }
     else printf("%s\n","Unable to open 600 again once closed.");
      

    int start = 0;
    for ( int i = 0; i < 10; i++ )
    {
		port = smk_new_port( smk_gettid() );
		if ( i == 0 ) start = port;
		printf("%i%s%i\n",i, " == ", port );
    }

	for ( int i = 0; i < 10; i++ )
	  smk_release_port( start + i );


	printf("%s\n","Creating a new port" );
	port = smk_new_port( smk_gettid() );
	printf("%s%i\n","A new port was created: ", port );
	smk_release_port( port );

	port = smk_create_port( 555, smk_gettid() );
	printf("%s%i\n","create_port(555) returned ", port );
	port = smk_create_port( 555, smk_gettid() );
	printf("%s%i\n","create_port(555) returned ", port );
    smk_release_port( 555 );	


   return 0;
 }
 
