#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <spoon/ipc/Message.h>
#include <spoon/app/ConsoleApplication.h>

// -----------------------------------------------

int32 main_stub( void *data )
{
	ConsoleApplication *master;
	master = (ConsoleApplication*)data;
	return master->main( 0, NULL );
}

// -----------------------------------------------


ConsoleApplication::ConsoleApplication(const char *signature)
: Application( signature )
{
	kbd_sem = smk_create_sem( 0, "kbd" );
	kbd_position = 0;
	kbd_count = 0;

	history_list = NULL;
	history_count = 0;
	history_position = 0;
}

ConsoleApplication::~ConsoleApplication()
{
	smk_delete_sem( kbd_sem );

	if ( history_list != NULL ) free( history_list );
}



void ConsoleApplication::PulseReceived( Pulse *p )
{

	switch ( (*p)[0] )
	{
		case KEY_DOWN:
			kbd_buffer[  (kbd_position + kbd_count) % 256 ] = (*p)[1]; 
			kbd_count++;
			smk_release_sem( kbd_sem );
			break;
		default:
			Application::PulseReceived( p );
			break;
	}
}

int ConsoleApplication::Run()
{
   main_tid = smk_spawn_thread( main_stub, "main", 0, (void*)this );
   smk_resume_thread( main_tid );
   return Application::Run();
}


char *ConsoleApplication::history(int i)
{
	if ( i < 0 ) return NULL;
	if ( i >= history_count ) return NULL;

	return history_list[i];
}

void ConsoleApplication::add_history( char* msg )
{
	history_count += 1;
  	history_list = (char**)realloc( history_list, history_count * sizeof(char**) );
  	history_list[history_count-1] = strdup( msg );
	history_position = history_count;
}


///  \todo Not really safe.... add a buffer lock mechanism.

// Basic console facilities..
int ConsoleApplication::getch()
{
	char a;
	if ( smk_acquire_sem( kbd_sem ) != 0 ) return 0;
	a = kbd_buffer[ kbd_position ];
	kbd_count--;
	kbd_position = (kbd_position + 1) % 256;
	return a;
}

//  Always assumes buffer is 256 chars long
int ConsoleApplication::gets( char *buffer  )
{
  char c;
  int pos;

  pos = 0;

  while (1==1)
  {
	c = getch();

	// ----------------------
		if ( (c == 72) && (history_position > 0) )
		{
			while (pos > 0 )
			{
	       		printf("%s","\b \b");
				buffer[--pos] = 0; 
			}
				
			history_position -= 1;
			strcpy( buffer, history_list[history_position] );
			pos = strlen( buffer );
			printf("%s", buffer );

			continue;
		}
		if ( c == 72 ) continue;
	
	// ----------------------
		if ( (c == 80) && (history_position < history_count) )
		{
			while (pos > 0 )
			{
	       		printf("%s","\b \b");
				buffer[--pos] = 0; 
			}
				
			history_position += 1;

			if ( history_position != history_count )
			{
				strcpy( buffer, history_list[history_position] );
				printf("%s", buffer );
			}
			
			pos = strlen( buffer );

			continue;
		}
		if ( c == 80 ) continue;
	

		

	if ( c == '\b' )
	{
		if ( pos > 0 )
		{
       		printf("%s","\b \b");
			buffer[--pos] = 0; 
		}
	} else
          	if ( c == '\n' )
         	{
	        	buffer[pos] = 0;
		        break;
        	}
	        else if (pos < 255)
		     {
				buffer[pos++] = c;
				printf("%c",c);
		     }
  }


  if ( pos > 0 ) add_history( buffer );
	
  history_position = history_count;
  
  return 0;
}




