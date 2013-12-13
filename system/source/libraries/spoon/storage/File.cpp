#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/storage/File.h>
#include <spoon/storage/StorageDefs.h>

File::File( const char *node )
: _fd(-1)
{
  _node = strdup(node);
}

File::~File()
{
  if ( _fd >= 0 ) Close();
  if ( _node != NULL ) free( _node );
}


// ---------------------------------------------------------


int File::Create( const char *node )
{

  Message *reply = NULL;
   Message *msg = new Message( CREATE );
            msg->AddString( "node", node );

	reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	delete msg;

	 if ( reply == NULL ) return -1;

   int ok = reply->what;
   delete reply;
   return ok;
}

int File::Delete( const char *node )
{
  Message *reply = NULL;
   Message *msg = new Message( DELETE );
            msg->AddString( "node", node );

	reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	delete msg;

	if ( reply == NULL ) return -1;

   int ok = reply->what;
   delete reply;

   return ok;

}

bool File::Exists( const char *node )
{
	File *file = new File( node );
	bool exists = ((file->Open() >= 0 ));
	if ( exists == true ) file->Close();
	delete file;
	return exists;
}




// ---------------------------------------------------------

int File::Open()
{
  Message *reply = NULL;
   Message *msg = new Message( OPEN );

            msg->AddString( "node", _node );

	 reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	 delete msg;

	 if ( reply == NULL ) return -1;

      if ( reply->FindInt32("_fd", &_fd ) != 0 ) _fd = -1;
      delete reply;

   return _fd;
}

int File::Read( void *data, int len )
{
   if ( _fd < 0 ) return -1;
   Message *msg = new Message(READ);

            msg->AddInt32("_fd", _fd);
	    msg->AddInt32("_len", len );

	Message *reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
    delete msg;

    if ( reply == NULL ) return -1;


	int rc;
	if ( reply->FindInt32("_rc", &rc) != 0 )
	 {
	   delete reply;
	   return -1;
	 }

	 if ( rc == 0 )
	  {
	    delete reply;
	    return 0;
	  }
	    
    void *newd;
    int news;
    if ( reply->FindData( "_data", RAW_TYPE,
    			  (const void**)&newd, &news) != 0 )
			   {
			     delete reply;
			     return rc;
			   }

      if ( news > len ) 
       {
         delete reply;
	 return -1;	// weird.  Given more data than requested.
       }
   
      memcpy( data, newd, news );
      delete reply;
      
      return news;
}

int File::Write( const void *data, int len )
{
   if ( _fd < 0 ) return -1;
   Message *msg = new Message(WRITE);

            msg->AddInt32("_fd", _fd);
	    msg->AddInt32("_len", len );
	    msg->AddData( "_data", RAW_TYPE, data, len );

	Message *reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	delete msg;
	if ( reply == NULL ) return -1;

	int rc;
	if ( reply->FindInt32("_rc", &rc) != 0 )
	 {
	   delete reply;
	   return -1;
	 }

	delete reply;
	return rc;
}

int File::Stat( struct stat *st )
{
  Message *reply = NULL;
	Message *msg = new Message(STAT);
                 msg->AddInt32("_fd", _fd );

		reply = Messenger::SendReceiveMessage( "gui_server", 0, msg );
        delete msg;	

    if ( reply == NULL ) return -1;

    if ( reply->what != OK ) 
     {
       delete reply;
       return -1;
     }

 
    void *newst;
    int size;
    if ( reply->FindData( "_stat", RAW_TYPE,
    			  (const void**)&newst, &size) != 0 )
	{
	     delete reply;
	     return -1;
	}

   memcpy( st, newst, sizeof( struct stat ) );
   delete reply;
   return 0;
}

int File::Close()
{
	/// \todo convert to message.
	Messenger::SendPulse( "vfs_server", 0, CLOSE, _fd );
   _fd = -1;
   return 0;
}


int64 File::Seek( int64 pos )
{
 Message *reply = NULL;
   Message *msg = new Message(SEEK);

     msg->AddInt32("_fd", _fd );
     msg->AddInt64("_seek", pos );

	 reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	 delete msg;
     if ( reply == NULL ) return -1;

     int rc = -1;

     if ( reply->FindInt32("_rc", &rc ) != 0 )
      {
        delete reply;
	return -1;
      }

     if ( rc != 0 ) return -1;
     
     return pos;
}

// ****************************************************

int64 File::Size()
{
   struct stat st;

     if ( Stat( &st ) != 0 ) return -1;

   return st.st_size;
}

//*****************************************************


bool File::Eject()
{
 Message *reply = NULL;
   Message *msg = new Message(EJECT);

     msg->AddInt32("_fd", _fd );

	 reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	 delete msg;

     if ( reply == NULL ) return false;

     int rc = reply->what;
     delete reply;

     if ( rc != 0 ) return false;
     
     return true;
}

bool File::Loaded()
{
 Message *reply = NULL;
   Message *msg = new Message(LOADED);

     msg->AddInt32("_fd", _fd );

	 reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	 delete msg;

	 if ( reply == NULL ) return false;

     int rc = reply->what;
     delete reply;

     if ( rc != 0 ) return false;
     
     return true;

}




