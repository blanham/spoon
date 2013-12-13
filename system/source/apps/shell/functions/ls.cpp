#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "ls.h"

#include "../Shell.h"
#include <spoon/storage/Directory.h>
#include <spoon/storage/File.h>

struct function_info info_ls = 
{
   "display the contents of a directory",
   "ls [path]\n"
   "This will display the contents of a path.",
   ls_main
};


 int ls_main( int argc, char **argv )
 {
    char *node = NULL;
    if ( argc == 1 ) node = strdup("/");
    if ( argc == 2 ) node = strdup( argv[1] );
    if ( node == NULL ) return -1;

    Directory *dir = new Directory( node );
    if ( dir->Exists() == false ) 
    {
      printf("%s\n","directory does not exist.");
    }
  
	char *front = (char*)malloc( strlen(node) + 2 );
		  strcpy( front, node );
		  if ( front[strlen(front) - 1] != '/' ) 
				strcat( front, "/" );

	
     char *entry = NULL;
     while ((entry = dir->NextEntry()) != NULL )
     {
	   char *complete = (char*)malloc(strlen(front) + strlen(entry) + 5 );

		   strcpy( complete, front );
		   strcat( complete, entry );
		   
		   int size = -1;
		   File *temp = new File( complete );
		   if ( temp->Open() >= 0 )
		   {
		     size = temp->Size();
			 temp->Close();
		   }
		   delete temp;
			 
       printf("%5s%10i  %s\n", ". ", size, entry );

	   free( complete );
     }
 
	 free( front );

    printf("%i%s\n", dir->CountEntries(), " files.");
    delete dir;
    free( node );
   
   return 0;
 }
 
