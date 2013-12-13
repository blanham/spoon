#include <types.h>
#include <stdio.h>
#include <string.h>
#include "../flist.h"
#include "help.h"

#include <spoon/app/ConsoleApplication.h>

struct function_info info_help = 
{
   "detailed help on commands",
   "help [command]\n"
   "If you need help on a specific function, type \"help\""
   " followed by the specific command you're interested in.",
   help_main
};


 int help_main( int argc, char **argv )
 {
    int i;
    int j;
    int handled;
    struct function_info *info;

    if ( argc == 1 ) // no parameters
    {
		for ( i = 0; i < TOTAL_FUNCTIONS; i++ )
		{
		   info = functions[i].info;
		   printf("%22s - %s\n", (functions[i]).command, info->summary );

		   if ( ((i+1)%10) == 0 )
		   {
			printf("%s\n","Press any key to continue...");
			((ConsoleApplication*)application)->getch();
		   }
		} 
    }
    else
    {
      for ( j = 1; j < argc; j++ ) // for each parameter
      {
        handled = -1;
		for ( i = 0; i < TOTAL_FUNCTIONS; i++ )  // find it.
		{
		   if ( strcmp( functions[i].command, argv[j] ) == 0 )
		   {
		    info = functions[i].info;
		    printf("%s%s\n", "COMMAND: ", (functions[i]).command );
		    printf("%s%s\n", "SUMMARY: ", info->summary );
		    printf("%s%s\n", "DESCRIPTION:\n", info->detailed );
		    handled = 0; 
		    break;
		   }
		}
		if ( handled == -1 ) printf("%s%s\n", argv[j]," - unknown command.");
      }
    }


   return -1;
 }
 
