#include <types.h>
#include <stdlib.h>
#include <stdio.h>

#include "EntityTable.h"
#include "Entity.h"


namespace VFS {

// --------------- HASH FUNCTIONS FOR THE ENTITY TABLE ------

int entity_key( void *data )
{
	Entity *entity = (Entity*)data;
	return (entity->getPid() * 1024 + entity->getFd());
}

int entity_cmp( void *a, void *b )
{
	Entity* entA = (Entity*)a;
	Entity* entB = (Entity*)b;

	if ( entA->getPid() < entB->getPid() ) return -1;
	if ( entA->getPid() > entB->getPid() ) return 1;
	
	if ( entA->getFd() < entB->getFd() ) return -1;
	if ( entA->getFd() > entB->getFd() ) return 1;
	
	return 0; 
}

// ----------------------------------------------------------


EntityTable::EntityTable()
: HashTable( entity_key, entity_cmp )
{
}
    
EntityTable::~EntityTable()
{
}

int EntityTable::getUniqueFD( int pid )
{
    int fd = 0;

	while ( getEntity( pid, fd ) != NULL )
	{
	  fd = ( fd + 1 ) % 32000;
	}

	return fd;
}

		     
Entity* EntityTable::getEntity( int pid, int fd )
{
	Entity sample( NULL, pid, fd );
	return (Entity*)retrieve( &sample );
}

    


}

