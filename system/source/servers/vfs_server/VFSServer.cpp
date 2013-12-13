#include <types.h>
#include <kernel.h>
#include <spoon/sysmsg.h>
#include <spoon/storage/StorageDefs.h>
#include <spoon/ipc/Messenger.h>
#include "VFSServer.h"

#include "Mount.h"

#include "fs/FileSystem.h"
#include "fs/list.h"

#include "open.h"

// **********************************************

FileTable file_table;
MountTable mount_table;
VFS::DeviceTable device_table;
VFS::EntityTable entity_table;


// **********************************************

VFSServer::VFSServer()
: Application("vfs_server")
{
}

// *********************************************

void VFSServer::installDevice( char *node, int pid, int port )
{
	VFS::Device* dev = new VFS::Device( node, pid, port, device_table.getNextDeviceID() );

   device_table.add( dev );

   printf("%s(%i,%i) %s\n", "Installed device: ", pid, port, node );
}

void VFSServer::uninstallDevice( int pid, int port )
{
	VFS::Device* dev = device_table.getDevice( pid, port );
  if ( dev == NULL ) return;

  printf("%s%s\n","Uninstalling ", dev->getName()  );

  if ( device_table.remove( dev ) == false ) return;

  delete dev;
}

// *********************************************

void VFSServer::doMount( Message *msg )
{
  char *point;
  char *node;
  char *fstype;

   if ( msg->FindString( "point", (const char**) &point ) != 0 )
  	   return;
   if ( msg->FindString( "node", (const char**) &node ) != 0 )
   {
   	free( point );
  	return; 
   }
   if ( msg->FindString( "fs", (const char**) &fstype ) != 0 )
   {
   	fstype = strdup("auto");
   }

   point = fixNode( -5, point, true );

   printf("%s%s%s%s%s%s\n", "Request to mount ", node, 
	    " onto ", point, " using ", fstype );

   VFS::Device* dev = NULL;
	if ( strncmp( node, "/device/", strlen("/device/") ) != 0  )
    	{
		free( node );
		free( point );
		free( fstype );
		printf("%s\n","The requested node was not a device");
    		// TODO: handle files.
	      return;
	}

        dev = device_table.getDevice( node + strlen("/device/") );
	if ( dev == NULL )
	{
		free( node );
		free( point );
		free( fstype );
		printf("%s\n","The requested node was not found!");
    		// TODO: handle files.
	      return;
	}

	printf("%s%i\n","Found a device with did = ", dev->getDeviceID() );
	FileSystem *fs = new_fs( fstype, dev );
	printf("%s\n","Successfully created FileSystem device." );
	if ( fs == NULL )
	 {
	   printf("%s\n","fs type unknown");
	   free( node );
	   free( point );
	   free( fstype );
	   return;
	 }

	if ( strcmp(fstype,"auto") != 0 )
	{
		printf("%s\n","Testing device." );
		if ( fs->Test() == FS_UNSUPPORTED )
		{
		   printf("%s\n","fs says it doesn't support that device");
		   free( node );
		   free( point );
		   free( fstype );
		   delete fs;
		   return;
		}
		
		printf("%s\n","The file system test was successful");
	}


      if ( fs->Init() != 0 )
       {
       	   printf("%s\n","file system init failed.");
	   free( node );
	   free( point );
	   free( fstype );
	   delete fs;
	   return;
       }
	
	printf("%s\n","The file system initialisation was successful");
    
	mount_table.AddMount( new VFS::Mount(point, fs) );

   free( node );
   free( point );
   free( fstype );

   msg->SendReply((unsigned int)OK);
}

void VFSServer::doUnmount( Message *msg )
{
   char *point;
   if ( msg->FindString( "point", (const char**) &point ) != 0 )
  	   return;

   point = fixNode( -5, point, true );

   VFS::Mount *mnt = mount_table.GetMount( point );
   printf("%s%s\n","Request to unmount ", point );
   free( point );

	if ( mnt == NULL )
	{
	  printf("%s\n","Unable to find mount point.");
	  msg->SendReply( (unsigned int)ERROR );
	  return;
	}

	// TODO: check if anyone is using it.
   mount_table.RemoveMount( mnt );

   delete mnt;
   msg->SendReply( (unsigned int)OK );
}


// *********************************************

void VFSServer::createFile( Message *msg, int pid, char *node )
{
   int len = strlen("/device/");

   if ( strncmp( node, "/device/", len ) == 0  ) return;

      VFS::Mount *mnt = mount_table.MatchMount( node );
      if ( mnt == NULL ) return;

      int ok = mnt->getFS()->Create( node + strlen(mnt->getPoint()) );
      if ( ok != 0 ) return;

	 msg->SendReply( (uint32) OK );
}

void VFSServer::deleteFile( Message *msg, int pid, char *node )
{
   int len = strlen("/device/");

   if ( strncmp( node, "/device/", len ) == 0  ) return;

      VFS::Mount *mnt = mount_table.MatchMount( node );
      if ( mnt == NULL ) return;

      int ok = mnt->getFS()->Delete( node + strlen(mnt->getPoint()) );
      if ( ok != 0 ) return;

	 msg->SendReply( (uint32) OK );

}

// *********************************************

void VFSServer::openFile( Message *msg, int pid, char *node )
{
	VFS::Entity *entity = open( pid, node );

	if ( entity == NULL )
	{
		printf("%s%i, %s\n","open returned NULL for ",pid, node );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	
	entity_table.add( entity );


	Message *reply = new Message(OK);
		 reply->AddInt32( "_fd", entity->getFd() );
	 	 msg->SendReply( reply );
	delete reply;
}




// **************************************************

void VFSServer::closeFile( Message *msg, int pid, int fd )
{
	VFS::Entity *entity = entity_table.getEntity( pid, fd );
	if ( entity == NULL ) 
	{
		printf("%s%i,%i\n","Unable to find file to close for ", pid, fd );
		//msg->SendReply( (unsigned int)-1);
		return;
	}

	entity_table.remove( entity );
	entity->Close();
	delete entity;
	

	return;

	/// \todo This should be a message.

	/*
	Message *reply = new Message(OK);
		 reply->AddInt32( "_rc", 0 );
	 	 msg->SendReply( reply );
	delete reply;
	*/
}

// **************************************************


void VFSServer::readFile( Message *msg, int pid, int fd )
{
	VFS::Entity *entity = entity_table.getEntity( pid, fd );
	if ( entity == NULL )
	{
		printf("%s%i, %i\n","read returned NULL for ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	
       
	int len = 0;
	if ( msg->FindInt32("_len", &len ) != 0 )
	 {
		printf("%s%i, %i\n","read without _len ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	 }

	if ( (len <= 0) || (len > (1024*1024*1024) ) ) 
	{
		printf("%s%i, %i, %i\n","length was too big or small ",len, pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}

		printf("%s%i, %i, %i\n","read: ",len, pid, fd );
	
     void *data = malloc(len);

	 int ans =  entity->Read( data, (long long)len );

	 if ( ans < 0 ) 
	  {
	    free( data );
	    msg->SendReply((unsigned int)ERROR);
	    return;
	  }
 	 

	 Message *re = new Message(OK);
	          re->AddInt32("_rc", ans );
	          re->AddInt32("_len", ans );
		  re->AddData("_data", RAW_TYPE, data, ans );
	 msg->SendReply( re );
	 free( data );
	 delete re;
}

// ****************************************************

void VFSServer::writeFile( Message *msg, int pid, int fd )
{
	VFS::Entity *entity = entity_table.getEntity( pid, fd );
	if ( entity == NULL )
	{
		printf("%s%i, %i\n","write returned NULL for ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	
       
	int len = 0;
	if ( msg->FindInt32("_len", &len ) != 0 )
	 {
		printf("%s%i, %i\n","read without _len ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	 }

	if ( (len < 0) || (len > (1024*1024*1024) ) ) 
	{
		printf("%s%i, %i, %i\n","length was too big ",len, pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}


	void *data;
	int size;
	if ( msg->FindData("_data", RAW_TYPE, (const void**)&data, &size ) != 0 )
	{
		printf("%s%i, %i, %i\n","could not find data: ", pid, fd );
   	  	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	

	int ans = entity->Write( data, len );

	 Message *re = new Message(OK);
	          re->AddInt32("_rc", ans );
		 	  msg->SendReply( re );
	 delete re;
}

// ********************************************

void VFSServer::seekFile( Message *msg, int pid, int fd )
{
	VFS::Entity *entity = entity_table.getEntity( pid, fd );
	if ( entity == NULL )
	{
		printf("%s%i, %i\n","write returned NULL for ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	
     int64 seek;

    if ( msg->FindInt64("_seek", &seek) != 0 ) 
	{
		printf("%s%i, %i\n","_seek not found in msg for ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}

	 seek = entity->Seek( seek );

     Message *reply = new Message(OK);
     	      reply->AddInt32("_rc", seek );
 	   msg->SendReply( reply );
     delete reply;

}

void VFSServer::statFile( Message *msg, int pid, int fd )
{
	VFS::Entity *entity = entity_table.getEntity( pid, fd );
	if ( entity == NULL )
	{
		printf("%s%i, %i\n","write returned NULL for ",pid, fd );
     	msg->SendReply( (unsigned int)-1 );	
		return;
	}
	
  struct stat st;
  
  int ans = entity->Stat( &st );
  if ( ans != 0 )
  {
     msg->SendReply((unsigned int)ERROR);
	 return;
  }

  Message *reply = new Message(OK);
  	       reply->AddInt32( "_rc", 0 );
  	       reply->AddData( "_stat", RAW_TYPE, &st, sizeof(struct stat));
		   msg->SendReply( reply );
		   delete reply;
}



// *********************************************

void VFSServer::ejectFile( Message *msg, int pid, int fd )
{
}

void VFSServer::loadedFile( Message *msg, int pid, int fd )
{
}



// *********************************************

void VFSServer::listRequest( Message *msg )
{
   char *node = NULL;

   if ( msg->FindString("_node", (const char**) &node ) != 0 )
   	return;

   node = fixNode( -5, node, true );

    	printf("%s%s\n","VFS list: ", node );

  
  	// any device listing.
   if ( strncmp( node, "/device/", strlen("/device/") ) == 0 ) 
    {
      listDevices( msg, node );
      free( node );
      return;
    }

   VFS::Mount *mnt = mount_table.MatchMount( node );
   if ( mnt == NULL ) 
   {
     if ( strcmp(node,"/") == 0 )
     	{
	   Message *reply = new Message(OK); 
		    reply->AddString("entry", "device" );
		    msg->SendReply( reply );
	     delete reply;
	}

   
     printf("%s\n","Unable to find an appropriate mount.");
     free( node );
     return;
   }

   node = fixNode( -5, node, false );

   printf("%s%s\n","asking for List: ", node + strlen(mnt->getPoint()) );
   Message *reply = NULL;
   int ans = mnt->getFS()->List( node + strlen(mnt->getPoint()), &reply );

   if ( ans != 0 )
   {
      free( node );
      return;
   }

	if ( strcmp(node,"") == 0 )
	   reply->AddString("entry", "device" );

	msg->SendReply( reply );

   delete reply;
   free( node );
}


void VFSServer::listDevices( Message *msg, char *path )
{
  printf("%s%s\n","Request to list devices: ", path );

  char *full = path + strlen("/device/");
  char *node = strdup(full);
  int   nlen = strlen(node);

  Message *reply = new Message(OK); 

  for ( int i = 0; i <  device_table.count(); i++ )
   {
		   VFS::Device* dev = (VFS::Device*)device_table.get(i);

     if ( strncmp( dev->getName(), node, nlen ) == 0 ) 
     {
     	printf("%s%s\n","focussed: ", dev->getName() );
		char *focus = strdup( dev->getName() + nlen );
		printf("%s%s\n","     sed: ", focus );

		 for ( unsigned int j = 0; j < strlen(focus); j++ )
		   if ( focus[j] == '/' ) 
		    {
		      focus[j] = 0;
		      break;
		    }

	printf("%s%s\n","     ---> ", focus );

	// sigh... sad search here.
	  bool found = false;
	  int k = 0;
	  char *fn = NULL;
	  while ( reply->FindString("entry", k++, (const char**)&fn ) == 0 )
	  {
	     if ( strcmp(fn,focus) == 0 ) 
	      {
	        free(fn);
		found = true;
		break;
	      }
	     free(fn);
	  }
	// ...............

	  if ( found == false ) reply->AddString("entry", focus );

	free( focus );
     }

   }

  msg->SendReply( reply );
  free( node );
}

// *********************************************

void VFSServer::makeDirectory( Message *msg )
{
   char *node = NULL;

   if ( msg->FindString("_node", (const char**) &node ) != 0 )
   	return;

   node = fixNode( -5, node, true );

    	printf("%s%s\n","VFS makeDirectory: ", node );

  
  	// any device listing.
   if ( strncmp( node, "/device/", strlen("/device/") ) == 0 ) 
    {
      free( node );
      return;
	  // Nah...
    }

   VFS::Mount *mnt = mount_table.MatchMount( node );
   if ( mnt == NULL ) 
   {
     printf("%s\n","Unable to find an appropriate mount.");
     free( node );
     return;
   }

   node = fixNode( -5, node, false );

   int ans = mnt->getFS()->MakeDirectory( node + strlen(mnt->getPoint()) );

   if ( ans != 0 )
   {
      free( node );
      return;
   }

	msg->SendReply( ans );
   free( node );
}



void VFSServer::deleteDirectory( Message *msg )
{
   char *node = NULL;

   if ( msg->FindString("_node", (const char**) &node ) != 0 )
   	return;

   node = fixNode( -5, node, true );

    	printf("%s%s\n","VFS deleteDirectory: ", node );

  
  	// devices can not be deleted.
   if ( strncmp( node, "/device/", strlen("/device/") ) == 0 ) 
    {
      free( node );
	  return;
	  // Nah...
    }

   VFS::Mount *mnt = mount_table.MatchMount( node );
   if ( mnt == NULL ) 
   {
     printf("%s\n","Unable to find an appropriate mount.");
     free( node );
     return;
   }

   node = fixNode( -5, node, false );

   int ans = mnt->getFS()->DeleteDirectory( node + strlen(mnt->getPoint()) );

   if ( ans != 0 )
   {
      free( node );
      return;
   }

   msg->SendReply( ans );
   free( node );

}


// *********************************************


char *VFSServer::fixNode( int pid, char *node, bool endingSlash, bool clean )
{
   if ( node == NULL ) return NULL;
   if ( strcmp(node,"") == 0 ) return node;

   int len = strlen( node );
   char *fixed = (char*)malloc( len + 10 );
   strcpy( fixed, node );

   if ( (fixed[len-1] != '/') && (endingSlash==true)) strcat( fixed, "/" );
   else
    if ( (fixed[len-1] == '/') && (endingSlash==false)) fixed[len-1] = 0;
   
  
   if ( clean == true ) free( node );
   return fixed;
}

// *********************************************

void VFSServer::MessageReceived( Message *msg )
{
  char *node;
  int pid;
  int port;
  int fd;

  pid = msg->source_pid();
  port = msg->source_port();
  
  printf("%s(%i)\n","VFS::Server MessageReceived", msg->what );
  
  switch( msg->what )
  {
    case INSTALL:
		//printf("%s\n","VFS::Server received an INSTALL request.");
        if ( msg->FindString( "node", (const char**) &node ) != 0 )
  	   break;
        if ( msg->FindInt32( "port", &port ) != 0 )
  	   break; 
		//printf("%s\n","VFS::Server parsed the correct data from the request.");
	
		installDevice( node, pid, port );
		free( node );
     break;
   
    case MOUNT:
        doMount( msg );
    	break;

    case UNMOUNT:
        doUnmount( msg );
    	break;
  
	case CREATE:
         if ( msg->FindString( "node", (const char**) &node ) != 0 )
  			   break;
	
		createFile( msg, pid, node );
		free( node );
		break;
 
	case DELETE:
         if ( msg->FindString( "node", (const char**) &node ) != 0 )
  			   break;
	
		deleteFile( msg, pid, node );
		free( node );
		break;

		
    case OPEN:
        if ( msg->FindString( "node", (const char**) &node ) != 0 )
  			   break;
	
		openFile( msg, pid, node );
		free( node );
		break;

    case WRITE:
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
		writeFile( msg, pid, fd );
        break;
 
    case READ:
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
		readFile( msg, pid, fd );
        break;
   
    case SEEK:
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
        seekFile( msg, pid, fd );
		break;

    case STAT:
		fd = -1;
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
        statFile( msg, pid, fd );
		break;

    case LIST:
    	printf("%s\n","VFS has received a list request.");
    	listRequest( msg );
    	break;

    case MKDIR:
    	makeDirectory( msg );
    	break;

    case RMDIR:
    	deleteDirectory( msg );
    	break;


		
    case LOADED:
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
		loadedFile( msg, pid, fd );
        break;

    case EJECT:
        if ( msg->FindInt32("_fd", &fd) != 0 ) break;
		ejectFile( msg, pid, fd );
        break;
   
    default:
     Application::MessageReceived( msg );
     break;
  }

  if ( msg->Replied() == false )		// TODO: Fix this..
     msg->SendReply( (unsigned int)-1 );	// Put it into code correctly

}

void VFSServer::PulseReceived( Pulse *p )
{
   uint32 d1,d2,d3,d4,d5,d6;
   int pid = p->source_pid;
   d1 = (*p)[0];
   d2 = (*p)[1];
   d3 = (*p)[2];
   d4 = (*p)[3];
   d5 = (*p)[4];
   d6 = (*p)[5];



   switch (d1)
   {
     case UNINSTALL:
       uninstallDevice( pid, d2 );	// d2 = port
       break;

     case CLOSE:
       closeFile( NULL, pid, d2 );
       break;
       
     default:
       Application::PulseReceived( p );
       break;
   }
   
}


