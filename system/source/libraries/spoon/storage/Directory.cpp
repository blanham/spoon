#include <stdio.h>
#include <string.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/storage/Directory.h>

Directory::Directory( const char *node )
: _node(NULL),_list(NULL), 
	  _pos(0),  _count(0)
{
   m_entry = NULL;
   Set( node );
}
    
Directory::~Directory()
{
  if ( _node != NULL ) free( _node );
  if ( m_entry != NULL ) free( m_entry );
  delete _list;
}

bool Directory::Set( const char *node )
{
   if ( _node != NULL ) free( _node );
   _node = strdup( node );
   Message *reply = NULL;
   Message *msg = new Message(LIST);
  	       msg->AddString("_node", _node );
		   reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
	   	   delete msg;
   _list = reply;
   _pos = 0;
   _count = _list->CountNames(STRING_TYPE);
   if ( _list->what == OK ) return true;
   return false;
}

bool Directory::Exists() 
{ 
	return (_list->what == OK); 
}

void Directory::Rewind() 
{ 
	_pos = 0; 
}

int Directory::CountEntries() 
{ 
	return _count; 
}
    
char *Directory::NextEntry()
{
   if ( m_entry != NULL ) free( m_entry );
   m_entry = NULL;
   
   if ( _pos >= _count ) return NULL;
   if ( _list->FindString("entry", _pos++, (const char**)&m_entry ) != 0 )
		    return NULL;
   
   return m_entry;
}

    

int Directory::Create( const char *node )
{
   Message *reply = NULL;
   Message *msg = new Message(MKDIR);
  	       msg->AddString("_node", node );
			 
		   reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg, TIMEOUT_10SEC );
		   if ( reply == NULL )
		 	{
				delete msg;
				printf("%s\n","TIMEOUT for MKDIR");
				return NULL;
			 }
			 
	   	   delete msg;

	int success = reply->what;
	delete reply;

	if ( success != OK ) return -1;
   return 0;
}



int Directory::Delete( const char *node )
{
   Message *reply = NULL;
   Message *msg = new Message(RMDIR);
  	       msg->AddString("_node", node );
			 
		   reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg, TIMEOUT_10SEC );
		     if ( reply == NULL )
			 {
				delete msg;
				printf("%s\n","TIMEOUT or FAILURE for RMDIR");
				return NULL;
			 }
			 
	   	   delete msg;

	int success = reply->what;
	delete reply;

	if ( success != OK ) return -1;
   return 0;
}




