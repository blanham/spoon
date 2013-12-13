#include <strings.h>
#include <dmesg.h>
#include <cpu.h>


int debug = 0;
int verbose = 0;


/** This function must NOT allocate memory or do
 * any memory operations. Directly or indirectly. 
 */
void parse_command_line( char *cmd_line )
{
	int last = 0;
	int pos = 0;
	int len = strlen( cmd_line );
	char *command;

	while ( pos < len )
	{
		while ( (pos<len) && (cmd_line[pos] == ' ') ) pos++;
		last = pos;
		while ( (pos<len) && (cmd_line[pos] != ' ') ) pos++;
		if ( last == pos ) break;

		cmd_line[pos] = 0;

		command = cmd_line + last;

		if ( strcmp( command, "help" ) == 0 )
		{
			dmesg("%!%s\n", "supported parameters: " );
			dmesg("%!%s\n", "   debug      prints debugging to screen.");
			dmesg("%!%s\n", "   help       this information.");
			dmesg("%!%s\n", "   verbose    turns on verbose bootup.");
			dmesg("%!%s\n", "" );
			dmesg("%!%s\n", "module parameters: ");
			dmesg("%!%s\n", "   env=name   module becomes name'd environment variable." );
			dmesg("%!%s\n", "   env_hosts=process1[,process2,..]" );
			dmesg("%!%s\n", "              the module is only mapped into the given processes'");
			dmesg("%!%s\n", "              environment maps. requires env to be specified.");

			while (1==1) { halt_cpu(); };
		}

		if ( strcmp( command, "debug" ) == 0 ) debug = 1;
		if ( strcmp( command, "verbose" ) == 0 ) verbose = 1;

		

		pos++;
	}

}

