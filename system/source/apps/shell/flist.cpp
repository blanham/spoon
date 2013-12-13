#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <spoon/app/System.h>

#include "flist.h"

#include "functions/cat.h"
#include "functions/clear.h"
#include "functions/cp.h"
#include "functions/crash.h"
#include "functions/dmesg.h"
#include "functions/help.h"
#include "functions/ifconfig.h"
#include "functions/ls.h"
#include "functions/mkdir.h"
#include "functions/more.h"
#include "functions/mount.h"
#include "functions/pci_list.h"
#include "functions/port_test.h"
#include "functions/ps.h"
#include "functions/reboot.h"
#include "functions/rm.h"
#include "functions/rmdir.h"
#include "functions/run.h"
#include "functions/route.h"
#include "functions/sin.h"
#include "functions/sleep.h"
#include "functions/spit.h"
#include "functions/tan.h"
#include "functions/test.h"
#include "functions/thread_test.h"
#include "functions/time.h"
#include "functions/tls_test.h"
#include "functions/touch.h"
#include "functions/unmount.h"

struct function_entry functions[] = 
{
   { "cat",  &info_cat },
   { "clear",  &info_clear },
   { "cp",  &info_cp },
   { "crash",  &info_crash },
   { "dmesg",  &info_dmesg },
   { "help",  &info_help },
   { "ifconfig",  &info_ifconfig },
   { "ls",  &info_ls },
   { "mkdir",  &info_mkdir },
   { "more",  &info_more },
   { "mount",  &info_mount },
   { "pci_list", &info_pci_list },
   { "port_test", &info_port_test },
   { "ps", &info_ps },
   { "reboot", &info_reboot },
   { "rm", &info_rm },
   { "rmdir", &info_rmdir },
   { "route", &info_route },
   { "run", &info_run },
   { "sin", &info_sin },
   { "sleep", &info_sleep },
   { "spit", &info_spit },
   { "tan", &info_tan },
   { "test", &info_test },
   { "thread_test", &info_thread_test },
   { "time", &info_time },
   { "tls_test", &info_tls_test },
   { "touch", &info_touch },
   { "unmount", &info_unmount }
};

/*
 * Attempt to execute a command line directly from the VFS server.
 */
int disk_execute( int argc, char *argv[], int wait )
{
    int append = 0;
    
    if ( argc < 1 ) return -1;
    if ( argv[0][0] == '/' ) append = -1;

     char *filename = 
     	(char*)malloc( strlen("/spoon/system/bin/") + strlen(argv[0]) + 1 );

        filename[0] = 0;

        if ( append == 0 )
		strcpy( filename, "/spoon/system/bin/" );
	
	strcat( filename, argv[0] );

	// Build up the command line parameters
     char *cmd_line = (char*)malloc(1024);
           cmd_line[0] = 0;
           for ( int i = 1; i < argc; i++ )
	     {
	       if ( i != 1 ) strcat( cmd_line, " " );
               strcat( cmd_line, argv[i] );
	     }


     int rc = System::Execute( filename, cmd_line, (wait == 0) );
     free( cmd_line );
     free( filename );

	 if ( wait == 1 )
	 {
		if ( rc > 0 )
		{
			printf("%s%i%s\n","(pid = ", rc, ")" );
			rc = 0;
		}
	 }
	 
     return rc;
}


/*
 *  Break down the command line and send it to the appropriate
 *  main functions as listed in functions[] above.
 */

char** parse_parameters( int *argc, char* params);

int function_call( char *command )
{
     char **argv;
     int argc;
     int i;
     int handled = -1;
     int rc = -1;
     struct function_info *info;

       argv = parse_parameters( &argc, command );

	if ( argv == NULL ) return -1;
	if ( argc == 0 ) return -1;

	int noWait = 0;
	if ( strcmp( argv[argc-1], "&" ) == 0 )
	{
		argc -= 1;
		noWait = 1;
	}
		// only &	
		if ( argc == 0 ) 
		{
			free( argv[0] );
			free( argv );
			return -1;
		}


	for ( i = 0; i < TOTAL_FUNCTIONS; i++ )
	{
	    if ( strcmp( (functions[i]).command, argv[0] ) == 0 )
	    {
	      info = functions[i].info;
	      rc = info->fmain( argc, argv );
	      handled = 0;
	      break;
	    }
	} 

	if ( handled == -1 ) 
	  handled = disk_execute( argc, argv, noWait );
	
	if ( handled == -1 )
	  printf("%s\n","Unknown command. Type \"help\" for a list.");

	if ( noWait == 1 ) argc += 1;
	
	// Destroy the parameter list we were given.
     for ( i = 0; i < argc; i++ ) free( argv[i] );
     free( argv );

     return handled;
}

/*
 *  Takes the command line (params) and returns an
 *  appropriate array with the number found in *argc
 */

char** parse_parameters( int *argc, char* params)
{
	char **parmlist;
	char *tmp;
	int pcount;
	int state;
	int i;
	int length;
	int start;


	if ( params == NULL ) 
	{
		*argc = 0;
		return NULL;
	}

	 length = strlen( params );
	 parmlist = NULL;

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

	   if (  ((params[i] == ' ')||( i == (length - 1) ))
	   	  && (state == 1) )
	   {
	        state = 0;
		if ( i == (length - 1) ) i += 1; // Not a space but
						 // the end.
		
		tmp = (char*)malloc( i - start + 1 ); 	// +1 for null
		memcpy( tmp, (void*)(params + start), i - start );
		tmp[ i - start ] = 0;
		parmlist = (char**)
			 realloc( parmlist, ++pcount * sizeof( char* ) );
		parmlist[ pcount - 1 ] = tmp;
		start = -1;
	   }

	 }

	// ------------------------
	

	*argc = pcount;
	return parmlist;
}



