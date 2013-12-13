#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "Shell.h"
#include <spoon/support/String.h>



#include <spoon/translators/PNGTranslator.h>

int main( int argc, char *argv[] )
{
	Shell *susan = new Shell();
	       susan->Run();
	delete susan;
  return 0;	
}



