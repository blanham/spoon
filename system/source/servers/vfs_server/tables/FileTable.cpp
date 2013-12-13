#include <types.h>
#include <stdlib.h>
#include <stdio.h>

#include "FileTable.h"

#include "../Mount.h"

FileTable::FileTable()
: hashSize(0), hash_table(NULL),
  hashCount(0), hash_list(NULL)
{
   Rehash( 1024 );
}
    
FileTable::~FileTable()
{
   if ( hash_table != NULL ) free( hash_table );
   if ( hash_list != NULL ) free( hash_list );
}

int FileTable::UniqueFD( int pid )
{
    int fd = 1;
	struct file_info *file = NULL;

	do
	{
	  fd = ( fd + 1 ) % 32000;
	  if ( fd == 0 ) fd = 1;
	    file = getFile( pid, fd );
	} while ( file != NULL );

	return fd;
}

int FileTable::insertFile( struct file_info * win )
{
     if ( (hashCount * 100 / hashSize) > 70 ) Rehash( hashSize * 4 );
      
     int new_pos = Hash( win->pid, win->fd, hashSize );

	// should be available but just in case.
     while ( hash_table[new_pos].pid != -1 )
       new_pos = ( new_pos + 1 ) % hashSize;

     hash_table[ new_pos ] = *win;
     hash_list[ hashCount ] = & (hash_table[new_pos]);

     hashCount += 1;

     return new_pos;
}
		    
int FileTable::FileCount() 
{ 
	return hashCount; 
}
		 
struct file_info *FileTable::getFile( int order )
{
    if ( order < 0 ) return NULL;
	if ( order >= hashCount ) return NULL;
    return hash_list[ order ];
}
		     
struct file_info *FileTable::getFile( int pid, int fd )
{
    int new_pos = Hash( pid, fd, hashSize );
    int checks = 0;

	// should be available but just in case.
     while ( ( hash_table[new_pos].pid != pid) ||
             ( hash_table[new_pos].fd != fd)  )
             {
         new_pos = ( new_pos + 1 ) % hashSize;
	 if ( checks++ > hashSize ) return NULL; // Looped around
             }

     return &(hash_table[ new_pos ]); 
}

    

int FileTable::removeFile( int pid, int fd )
{
   	// Remove from the hash table.
    struct file_info *win = getFile( pid, fd );
    if ( win == NULL ) return -1;
    win->pid = -1;

  	// Now from the hash_list
    for ( int i = 0; i < hashCount; i++ )
      if ( hash_list[i] == win )
      {
		  for ( int j = i; j < hashCount; j++ )
		    hash_list[j] = hash_list[j + 1];
		    
	   break;  
      }


	hashCount -= 1;
	 
    return 0; 
}
      


int FileTable::Hash( int pid, int fd, unsigned int size )
{
   return ( (pid * 64 + fd) % size );
}

int FileTable::Rehash( int newSize )
{
    // allocate it
    struct file_info *new_table =
	  (struct file_info*)malloc( newSize * sizeof( struct file_info ) );

	// clean it.	
	for ( int i = 0; i < newSize; i++ ) new_table[i].pid = -1;
	
	// rehash the old ones into new_table from the existing hash_list
	for ( int i = 0; i < hashCount; i++ )
	{
	  struct file_info *win = hash_list[i];
	  int new_pos = Hash( win->pid, win->fd, newSize );

		// next slot... 
	  while ( new_table[new_pos].pid != -1 )
		       new_pos = ( new_pos + 1 ) % newSize;

	  new_table[ new_pos ] = *win;
	  hash_list[i] = & (new_table[ new_pos ]);
	}

	if ( hash_table != NULL ) free( hash_table );
	hash_table = new_table;
	hashSize = newSize;

	// create the list for z-order.
	hash_list = (struct file_info**)realloc( hash_list, 
	                           newSize * sizeof(struct file_info*) );
	for ( int i = hashCount; i < hashSize; i++ )
	 hash_list[i] = NULL;

	return 0;
}



