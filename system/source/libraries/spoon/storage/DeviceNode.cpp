#include <types.h>
#include <stdio.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/storage/DeviceNode.h>


DeviceNode::DeviceNode( char *node )
{
   _node = strdup( node );
}

DeviceNode::~DeviceNode()
{
  if ( _node != NULL ) free( _node );
}

// ***************************************************************

void DeviceNode::Install()
{
   Message *msg = new Message(INSTALL);

     msg->AddString( "node", _node );
     msg->AddInt32( "port", Port() );

     int rc = Messenger::SendMessage( "vfs_server", 0, msg );

	 if ( rc != 0 )
	 {
		rc = 5;
	 }
//	 printf("%s(%s) %i\n","DeviceNode::Install", _node, rc );

   delete msg;
}

void DeviceNode::Uninstall()
{
	Messenger::SendPulse( "vfs_server", 0,  UNINSTALL, Port() );
}

int DeviceNode::read( int64 pos, void *buffer, int len )
{
	return -1;
}

int DeviceNode::write( int64 pos, void *buffer, int len )
{
	return -1;
}

int DeviceNode::stat( struct stat *st )
{
	return -1;
}

int DeviceNode::packet( int cmdSize, void *cmd, int dir,
       			   long bufSize, void* buffer,
			   unsigned long lba )
{
	return -1;
}

// ********************************************************

void DeviceNode::MessageReceived( Message *msg )
{
  void *data;
  int64 seek;
  int ans;
  int size;
  Message *reply;
  struct stat *st;
  
  switch (msg->what)
  {
    case READ:
      if ( msg->FindInt64( "_seek", &seek ) != 0 )  break;
      if ( msg->FindInt32( "_len", &size ) != 0 )  break;

      data = malloc( size );

		printf("%s( %i, %x, %i )\n", "DeviceNode::READ", (int)seek, data, size );
	  
      ans = read( seek, data, size );
      if ( ans < 0 ) 
      {
        free( data );
		msg->SendReply( (uint32)-1 );
        break;
      }

        reply = new Message( OK );
		reply->AddInt32( "_rc", ans );
		if ( ans > 0 ) reply->AddData("_data", RAW_TYPE, data, ans );
	  	    msg->SendReply( reply );
		delete reply;

      free( data );
     break;
 
  
    case WRITE:
      if ( msg->FindInt64( "_seek", &seek ) != 0 )  break;
      if ( msg->FindData( "_data", RAW_TYPE, 
      			  (const void**)&data, &size ) != 0 )  break;

		printf("%s( %i, %x, %i )\n", "DeviceNode::WRITE", (int)seek, data, size );
	  
		ans = write( seek, data, size );

		reply = new Message( OK );
		reply->AddInt32( "_rc", ans );
		      msg->SendReply( reply );
		delete reply;
	 break;
  
	case STAT:
		st = (struct stat*) malloc(sizeof(struct stat));

		ans = stat( st );

		reply = new Message(OK);
		reply->AddInt32( "_rc",  ans );
  	    reply->AddData( "_stat", RAW_TYPE, st, sizeof(struct stat));
			msg->SendReply( reply );
		delete reply;

		free( st );
	  break;
	 
	 
    case EJECT:
      ans = eject();
      reply = new Message(ans);
          msg->SendReply( reply );
      delete reply;
      break;

    case LOADED:
      ans = eject();
      reply = new Message(ans);
          msg->SendReply( reply );
      delete reply;
      break;
 

    default:
      Looper::MessageReceived( msg );
      break;
  }
}

   


