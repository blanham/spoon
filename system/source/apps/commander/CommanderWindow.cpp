#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#include "CommanderWindow.h"

#include <spoon/app/System.h>
#include <spoon/storage/Directory.h>
#include <spoon/storage/File.h>

#include <spoon/interface/Rect.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Button.h>
#include <spoon/interface/TextInput.h>
#include <spoon/interface/StringItem.h>


CommanderWindow::CommanderWindow( Rect frame )
: Window( frame, "commander"  )
{
	
	View *view			= NULL;
	Button *button 		= NULL;
   
	// Set up the top bar.
	view = new View( Rect(0,0,Bounds().Width() - 1, 49 ), FOLLOW_LEFT_RIGHT ); 
	view->SetBackgroundColour( 0x404040 );


		button = new Button( "UP", new Pulse( PULSE_UP ), Rect( 10,10, 40, 40 ) );	
		view->AddChild( button );
	/*
		button = new Button( "BACK", new Pulse( PULSE_BACK ), Rect( 10,10, 40, 40 ) );	
		view->AddChild( button );
	*/

	/*
		button = new Button( "UP", new Pulse( PULSE_UP ), Rect( 60,10, 90, 40 ) );	
		view->AddChild( button );
	*/

		button = new Button( "Refresh", new Pulse( PULSE_REFRESH ), Rect( Bounds().Width() - 50,10,  Bounds().Width() - 10, 40 ) );	
		view->AddChild( button );

		  
	AddChild( view );


	// Set up the URL bar.
	
	view = new View( Rect(0,50,Bounds().Width() - 1, 74), FOLLOW_LEFT_RIGHT );
	view->SetBackgroundColour( 0x303030 );


		input = new TextInput( Rect( 17, 2, Bounds().Width() - 38, 22 ), FOLLOW_LEFT_RIGHT );
		input->SetText( "/" );
		view->AddChild( input );

		button = new Button( "GO", new Pulse( PULSE_LOAD ), Rect( Bounds().Width() - 37, 2, Bounds().Width() - 17, 22 ) );
		view->AddChild( button );

	AddChild( view );

	// Set up the file list view

	list = new ListView( 
					Rect(0,75,Bounds().Width() - 1, Bounds().Height() - 1), 
					new Pulse( PULSE_SELECT ),
					FOLLOW_LEFT_RIGHT );
	list->SetBackgroundColour( 0x303050 );
	

	AddChild( list );
}

// -------------


bool CommanderWindow::Up()
{
	cleanPath();

	if ( strcmp( input->Text(), "/" ) == 0 ) return true;

	char *path = strdup( input->Text() );
	int len = strlen( path ) - 2;

		while ( len >= 0 )
		{
			if ( path[len] == '/' ) break;
			path[len] = 0;
			len -= 1;
		}
	
	if ( len < 0 ) input->SetText("/");
	else
	{
		input->SetText( path );
	}
		

	free( path );
	return true;
}

// -------------

bool CommanderWindow::Refresh()
{
	cleanPath();
	list->Empty();
		
	const char *path = input->Text();

	Directory *dir = new Directory( path );
    if ( dir->Exists() == false )  
    {
	  delete dir;
	  Draw( Bounds() );
	  return false;
    }
  
     char *entry = NULL;
     while ((entry = dir->NextEntry()) != NULL )
     {

	   char *complete = (char*)malloc( strlen(path) + strlen(entry) + 2 );

		   strcpy( complete, path );
		   strcat( complete, entry );
		   
		   int size = -1;
		   File *temp = new File( complete );
		   if ( temp->Open() >= 0 )
		   {
		     size = temp->Size();
			 temp->Close();
		   }
		   delete temp;
			 

	   list->AddItem( new StringItem( entry ) );
	   free( complete );
     }

	Draw( Bounds() );
    delete dir;

	return true;
}

// ......

bool CommanderWindow::Select()
{
	StringItem *item = (StringItem*)list->Selected();
	if ( item == NULL ) return false;
	
	const char *path = input->Text();
	const char *file = item->Label();

	// .........
	
	if ( strcmp( file, "." ) == 0 ) return true; 
	if ( strcmp( file, ".." ) == 0 ) return Up(); 
	
	
	// .........

	char *result = (char*)malloc( strlen(path) + strlen( file ) + 1 );

	strcpy( result, path );
	strcat( result, file );

	
	bool isDir = true;

		// ....... can handle? .........
	
			char *command;
			char *params;
			if ( canHandle( result, &command, &params ) == true )
			{
				char *answer = (char*)malloc( strlen(result) + 
											  strlen(params) +
											  3 );

				strcpy( answer, params );
				strcat( answer, " " );
				strcat( answer, result );

				/*
				printf("%s \"%s\" with \"%s\".\n",
							"executing: ", 
							command, 
							answer
							);
							*/

				System::Execute( command, answer, false );

				free( answer );

				isDir = false;
			}

		// .............................
	
		if ( isDir == true ) input->SetText( result );

	free( result );
	
	return true;
}

// ......

void CommanderWindow::PulseReceived( Pulse *pulse )
{
		
	switch ( (*pulse)[0] )
	{
		case PULSE_BACK:
		case PULSE_UP:
				if ( Up() )
						Refresh();
				break;
			
		case PULSE_REFRESH:
		case PULSE_LOAD:
				Refresh();
				break;

		case PULSE_SELECT:
				if ( Select() )
						Refresh();
				break;

		default:
			Window::PulseReceived( pulse );
			break;
	}
}





// ----------


bool CommanderWindow::cleanPath()
{
	if ( input->Text() == NULL )
	{
		input->SetText("/");
		return true;
	}

	char *msg = strdup( input->Text() );
	char *tmp = msg;

	// Strip leading and trailing whitespace.
		while ( (*tmp != 0) && (isspace( *tmp )) ) tmp++;


		if ( *tmp == 0 )	// It was all whitespace.
		{
			input->SetText("/");
			free( msg );
			return true;
		}

	// Strip trailing whitespace

		int len = strlen( tmp );

		while ( (len-1 >= 0) && (isspace(tmp[len-1])) )
		{
			*tmp = 0;
			len--;
		}
		
		if ( len < 0 ) 
		{
			input->SetText("/");
			free( msg );
			return true;
		}	
	
	// Add the trailing forward slash if not present.
		
	if ( tmp[ strlen(tmp) - 1] != '/' )
	{
		char *msg2 = (char*)malloc( strlen( tmp ) + 2 );	// Plus '/', Plus 0
		
		strcpy( msg2, tmp );
		strcat( msg2, "/" );

		free( msg );

		msg = msg2;
		tmp = msg2;
	}

	input->SetText( tmp );
	
	free( msg );
	return false;
}




bool CommanderWindow::isDirectory( const char *node )
{
	File *file = new File( node );

	if ( file->Open() < 0 ) 
	{
		delete file;
		return false;
	}

	struct stat st;

		if ( file->Stat( &st ) != 0 )
		{
			file->Close();
			delete file;
			return false;
		}

	bool ans = S_ISDIR( st.st_mode );

	file->Close();
	delete file;
	return ans;
}



#define COMMAND_MPEG  	"/spoon/system/bin/mpeg2decode"
#define PARAMS_MPEG 	"-f -r -o6 -b"
#define COMMAND_PNG   	"/spoon/system/bin/pngview"
#define PARAMS_PNG 		""


bool CommanderWindow::canHandle( const char *filename, char **command, char **params )
{
	int len = strlen( filename );
	
	// ----------------------------------------------

	if ( len < 4 ) return false;	// ".mpg"

	if ( strcasecmp( filename + len - 4, ".mpg" ) == 0 )
				{
					*command = COMMAND_MPEG;
					*params = PARAMS_MPEG;
					return true;
				}
			
	if ( strcasecmp( filename + len - 4, ".png" ) == 0 )
				{
					*command = COMMAND_PNG;
					*params = PARAMS_PNG;
					return true;
				}
	
	// ----------------------------------------------
	
	if ( len < 5 ) return false;
	
	
	if ( strcasecmp( filename + len - 5, ".mpeg" ) == 0 )
				{
					*command = COMMAND_MPEG;
					*params = COMMAND_PNG;
					return true;
				}

	
	
		
	return false;
}








