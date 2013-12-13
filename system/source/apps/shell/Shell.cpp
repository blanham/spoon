#include <types.h>
#include <string.h>
#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include "Shell.h"
#include "flist.h"


Shell *shell = NULL;

Shell::Shell()
: ConsoleApplication("Shell")
{
   shell = this;
}

Shell::~Shell()
{
   shell = NULL;
}


int Shell::main( int argc, char *argv[] )
{
	char command[256];
	
	printf("\n%s\n",
		"This is a simple shell for a spoon system.\n"
		"Type \"help\" for a list of available commands." );
	

	add_history( "/spoon/system/drivers/storage/ramdisk 64 &");
	add_history( "mkfs.djfs /device/storage/ramdisk0");
	add_history( "mount /device/storage/ramdisk0 /RAM auto" );
	add_history( "cp /spoon/system/bin/mpeg2decode /RAM/play" );
	add_history( "cp /spoon/system/media/1.mpg /RAM/1.mpg" );
	add_history( "/RAM/play -f -r -o6 -b /RAM/1.mpg" );
	add_history( "mpeg2decode -f -r -o6 -b /spoon/system/media/1.mpg &" );

	// -------------------------------------

	while (1)
	{
		printf("%s","> ");
		gets(command);
		printf("%s","\n");

		function_call( command );	
	}

	return 0;
}



