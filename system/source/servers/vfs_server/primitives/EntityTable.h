#ifndef _VFS_ENTITYTABLE_H
#define _VFS_ENTITYTABLE_H

#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <spoon/collections/HashTable.h>

#include "Entity.h"

namespace VFS {

class EntityTable : public HashTable
{
  public: 
    EntityTable();
   ~EntityTable();

     int getUniqueFD( int pid );

     Entity* getEntity( int pid, int fd );

};

}

#endif


