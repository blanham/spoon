#include <stdio.h>
#include <kernel.h>



#include "TextualDriver.h"


// **********************************************************

int main(int argc, char *argv[] )
{
	printf("%s\n", "textual - video driver.");

	  TextualDriver *textual = new TextualDriver( );
	  			  	 textual->Resume();


	  int rc;
	  
	  if ( smk_wait_thread( textual->tid(), &rc ) != 0 )
		printf("%s: %i\n","problem waiting for thread", textual->tid() );
	  else
	  	printf("%s%i\n","textual video driver returned with: " , rc );
	  
					
	return 0;
}


