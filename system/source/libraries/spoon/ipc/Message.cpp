#include <types.h>
#include <string.h>
#include <stdlib.h>

#include <spoon/sysmsg.h>
#include <spoon/support/String.h>
#include <spoon/interface/Rect.h>
#include <spoon/ipc/Message.h>
#include <spoon/ipc/Messenger.h>


/*** The actual structure inserted into the Collection for good data, 
 * maintenance. 
 */
struct message_data
{
	char name[MESSAGE_NAME_LENGTH];
	type_code type;
	int32 size;
	char data;
} __attribute__ ((packed));


/** This constructor creates an empty Message with the what value set to 
 * command. (that does make sense.)
 *
 * \param command used to set what on construction? 
 */
Message::Message( uint32 command )
{
	what = command;
	_source_pid   = -1;
	_source_port  = -1;
	_dest_port    = -1;
	_replied      = false;
}

/** A copy constructor which relies on the "=" operator to
 * set up this Message as the message passed in.
 *
 * \warning This doesn't copy any data referenced by pointers, just
 * solid data which exists in the message: like ints, floats, rects, etc.
 * 
 * \param message The message to base this message on.
 */
Message::Message(const Message &message)
{
	*this = message;
}

/** Empty constructor which creates an empty Message. */
Message::Message(void)
{
	what = 0;
	_source_pid   = -1;
	_source_port  = -1;
	_dest_port    = -1;
	_replied      = false;
}

/** The desctructor deletes any message_data structures
 * used to maintain the message but not any data referenced
 * by the message_data structures. So it's up to you to clean
 * your messages up if they contain references to data.
 */
Message::~Message()
{
  int count = data_list.count();

  for ( int i = 0; i < count; i++)
   {
      struct message_data *tmp = 
            (struct message_data*) data_list.get( 0 );
	  data_list.remove( tmp );
      free( tmp );
   }

}


/** Convenience method to return the stored Int named
 * "rc". It's the equivalent of returning the second
 * parameter of FindInt("rc", &param);
 *
 * \return the value of the "rc" named Int.
 */ 
int Message::rc()
{
	int ans;
	if ( FindInt("rc", &ans ) != 0 ) return -1;
	return ans;
}

/** Convenience method to return the string referenced
 * by the "_error" data member. It uses a local static
 * buffer to store the message. 
 *
 * \warning Not thread safe if the "_error" value changes.
 * 
 * \return A pointer to a static char[1024] buffer.
 */
char *Message::error()
{
	char *str;
	static char message[1024];
	if ( FindString("_error", (const char**)&str) != 0 )
		return NULL;

	strncpy( message, str, 1023 );
	message[1023] = 0;
	free( str );
	return message;
}


/** Convenience method to sends a reply Message with a 
 * what value of "OK", and the given "rc" and "_error"
 * parameters.
 *
 * \param rc The return code which will be embedded as an Int named "rc".
 * \param error The return string to embed as "_error". Nothing is embedded
 * 		  if NULL is specified.
 *
 * 	\return 0 if the reply was sent successfully.
 */
int Message::SendRC( int rc, char *error )
{
	Message *msg = new Message( OK );
			 msg->AddInt( "rc", rc );
			 if ( error != NULL )  msg->AddString( "_error", error );
	int ans = SendReply( msg );
	delete msg;
	return ans;
}


/** Add's a bit of data to the Message. All the other methods
 * are stubs for this method. The method creates a new
 * message_data structure and stores the data, in that 
 * struct, in the local Collection.
 *
 * \warning You can't add NULL data.
 *
 * \param name The name to embed the data as.
 * \param type The type code to reference the data as.
 * \param data A pointer to the data to be embedded.
 * \param numBytes The number of bytes to embed.
 * \param fixedSize This has no actual point. 
 * \param numItems Yeah. No point either.
 *
 * \return 0 if the data was successfully added.
 */
int32 Message::AddData(const char *name, type_code type, 
		         const void *data, 
		         ssize_t numBytes, 
		         bool fixedSize, 
		         int32 numItems)

{
	struct message_data *md;

	if ( data == NULL ) return -1;

	md = (struct message_data*)
	          malloc( numBytes + sizeof(struct message_data) );
		  
	memcpy( md->name, name, MESSAGE_NAME_LENGTH );
	md->type = type;
	md->size = numBytes;
	memcpy( &(md->data), data, numBytes );

	data_list.add(md);

	return 0;
}

int32 Message::AddBool(const char *name, bool aBool)
{
	return AddData( name, BOOL_TYPE, &aBool, sizeof(bool) );
}

int32 Message::AddInt(const char *name, int anInt)
{
	return AddData( name, INT_TYPE, &anInt, sizeof(int) );
}


int32 Message::AddInt8(const char *name, int8 anInt8)
{
	return AddData( name, INT8_TYPE, &anInt8, sizeof(int8) );
}

int32 Message::AddInt16(const char *name, int16 anInt16)
{
	return AddData( name, INT16_TYPE, &anInt16, sizeof(int16) );
}

int32 Message::AddInt32(const char *name, int32 anInt32)
{
	return AddData( name, INT32_TYPE, &anInt32, sizeof(int32) );
}

int32 Message::AddInt64(const char *name, int64 anInt64)
{
	return AddData( name, INT64_TYPE, &anInt64, sizeof(int64) );
};

int32 Message::AddFloat(const char *name, float aFloat)
{
	return AddData( name, FLOAT_TYPE, &aFloat, sizeof(float) );
};

int32 Message::AddDouble(const char *name, double aDouble)
{
	return AddData( name, DOUBLE_TYPE, &aDouble, sizeof(double) );
};

int32 Message::AddString(const char *name, const char *string)
{
	return AddData( name, STRING_TYPE, string, strlen(string) + 1 );
};

int32 Message::AddString(const char *name, const String &string)
{
	return AddString( name, ((String)string).GetString() );
};

int32 Message::AddRect(const char *name, Rect rect )
{
    int data[4];
    data[0] = rect.left;
    data[1] = rect.top;
    data[2] = rect.right;
    data[3] = rect.bottom;
    return AddData(name,RECT_TYPE, data, sizeof(int) * 4 );
}


int32 Message::AddMessage(const char *name, const Message *message)
{
	char *buffer;
	int32 size;

	size = message->FlattenedSize();
	buffer = (char*)malloc( size );
	message->Flatten(buffer);

	int rc = AddData( name, MESSAGE_TYPE, buffer, size );

	free( buffer );
	return rc;
};


int32 Message::AddPointer(const char *name, const void *pointer)
{
	return AddData( name, POINTER_TYPE, &pointer, sizeof(void*) );
};


/** This returns the number of data elements in the message which are
 * of the type sepecified.
 *
 * \param type The type of data to count.
 *
 * \return The count of data elements.
 */
int32 Message::CountNames(type_code type) const
{
	int i;
	int counter;
	struct message_data *md;

	counter = 0;

	for ( i = 0; i < data_list.count(); i++)
	{
		md = (struct message_data*)data_list.get(i);
		if ( type == md->type ) counter++;
	}

	return counter;
}


/** This is almost the opposite of the AddData method. This returns
 * a pointer to the data which has been added with the specified
 * name and type, as well as the size of the data.
 *
 * All other methods also act as stubs for this method and translate
 * the data back into the formats requested.
 *
 * name and type and index have to match positively for this method
 * to consider it a match.
 *
 * \param name The name to look for.
 * \param type The type to look for.
 * \param index The index'th matching data element of the right type
 * 				and name to match.
 * \param data A pointer to a pointer to the correct data.
 * \param numBytes the size of the data is returned in the parameter.
 *
 * \return 0 on success.
 */

int32 Message::FindData(const char *name, 
	 	          type_code type, 
		          int32 index, 
		          const void **data, 
		          ssize_t *numBytes) const
{
	int i;
	int32 count;
	int32 number;
	struct message_data *md;

	number = 0;

	count = data_list.count();
	for ( i = 0; i < count; i++)
	{
		md = (struct message_data*)data_list.get(i);
		if ( type == md->type )
		  if ( strncmp( name, md->name, MESSAGE_NAME_LENGTH ) == 0 )
		    number++;

	        if ( number > index )
		{
			*data = (const void*)(&(md->data));
			*numBytes = md->size;
			return 0;
		}
	}

	return -1;
}


int32 Message::FindData(const char *name, 
		          type_code type, 
		          const void **data, 
		          ssize_t *numBytes) const
{
	return	this->FindData( name, type, 0, data, numBytes );
}



int32 Message::FindBool(const char *name, 
		        int32 index, 
		        bool *aBool) const
{
	int32 num;
	bool *ans;
	int32 k;
	k =  FindData( name, BOOL_TYPE, index, 
	                  (const void**)(&ans), &num );

	if ( k != 0 ) return k;

	*aBool = *ans;
	return k;
}

int32 Message::FindBool(const char *name, bool *aBool) const
{
	return FindBool( name, 0, aBool );
}


int32 Message::FindInt(const char *name, 
				        int32 index, 
			            int *anInt) const
{
	int num;
	int *ans;
	int32 k;
	k =  FindData( name, INT_TYPE, index, 
	                  (const void**)(&ans), &num );

	if ( k != 0 ) return k;

	*anInt = *ans;
	return k;

}

int32 Message::FindInt(const char *name, 
			      int *anInt) const
{
	return FindInt( name, 0, anInt );
}


/*
	int32 Message::FindInt8(const char *name, 
			      int32 index, 
			            int8 *anInt8) const;
	int32 Message::FindInt8(const char *name, 
			      int8 *anInt8) const;
	int32 Message::FindInt16(const char *name, 
			      int32 index, 
			            int16 *anInt16) const;
	int32 Message::FindInt16(const char *name, int16 *anInt16) const;
*/
	
int32 Message::FindInt32(const char *name, 
	  	         int32 index, 
	                 int32 *anInt32) const
{
	int32 num;
	int32 *ans;
	int32 k;
	k =  FindData( name, INT32_TYPE, index, 
	                  (const void**)(&ans), &num );

	if ( k != 0 ) return k;

	*anInt32 = *ans;
	return k;
}


int32 Message::FindInt32(const char *name, int32 *anInt32) const
{
  return FindInt32( name, 0, anInt32);
}

int32 Message::FindInt64(const char *name, int32 index, int64 *anInt64) const
{
	int num;
	int64 *ans;
	int k;
	k =  FindData( name, INT64_TYPE, index, 
	                  (const void**)(&ans), &num );

	if ( k != 0 ) return k;

	*anInt64 = *ans;
	return k;

}

int32 Message::FindInt64(const char *name, int64 *anInt64) const
{
  return FindInt64( name, 0, anInt64 ); 
}

/*
	int32 Message::FindFloat(const char *name, 
			      int32 index, 
			            float *aFloat) const;
	int32 Message::FindFloat(const char *name, float *aFloat) const;
	int32 Message::FindDouble(const char *name, 
			      int32 index, 
			            double *aDouble) const;
	int32 Message::FindDouble(const char *name, double *aDouble) const;

*/

int32 Message::FindString(const char *name, 
			      int32 index, 
			            const char **string) const
{
	char *data;
	int32 num;
	int32 k;
	
	k =  FindData( name, STRING_TYPE, index, (const void**)(&data), &num );

	if ( k != 0 ) return k;

	*string = strdup( data );

	return k;
}

int32 Message::FindString(const char *name, const char **string) const
{
	return FindString( name, 0, string );
}



/*
	int32 Message::FindString(const char *name, String *string) const;
	int32 Message::FindString(const char *name, int32 index, String *string) const;
*/


int32 Message::FindRect(const char *name, Rect *rect) const
{
	return FindRect(name, 0, rect);
}

int32 Message::FindRect(const char *name, int32 index, Rect *rect) const
{
	int *data;
	int32 num;
	int32 k;
	
	k =  FindData( name, RECT_TYPE, index, (const void**)(&data), &num );

	if ( k != 0 ) return k;

	rect->left   = data[0];
	rect->top    = data[1];
	rect->right  = data[2];
	rect->bottom = data[3];

	return k;
}


int32 Message::FindMessage(const char *name, 
			   int32 index, 
			   Message *message) const
{
	char *buffer;
	int32 k;
	int32 num;
	k =  FindData( name, MESSAGE_TYPE, index, 
	                  (const void**)(&buffer), &num );

	if ( k != 0 ) return k;

	message->Unflatten( buffer );

	return k;

}

int32 Message::FindMessage(const char *name, Message *message) const
{
	return FindMessage( name, 0, message );
}

/*
	int32 Message::FindPointer(const char *name, 
			      int32 index, 
			            void **pointer) const;
	int32 Message::FindPointer(const char *name, void **pointer) const;
*/

/** Flattens the internal Message data structures into one,
 * large, flat block of memory - perfect for sending somewhere
 * else.
 *
 * \param address The place to flatten the message into.
 * \param numBytes The size of the address buffer.
 * 
 * \return the size of the flattened block.
 */

int32 Message::Flatten(char *address, ssize_t numBytes ) const
{
	void *position;
	int32 i;
	int32 size;
	struct message_data *mp;

	position = address;
	if ( address == NULL )
	{
//		//printf("%s\n","flatten was given null");
		return -1;
	}

//	//printf("%s\n","about to flatten");

	memcpy( position, "MESSAGE", strlen("MESSAGE") );
	position = (void*)((uint32)position + strlen("MESSAGE") );
	memcpy( position, &what, sizeof(what) );
	position = (void*)((uint32)position + sizeof(what));
	i = data_list.count();
	memcpy( position, &i, sizeof(i) );
	position = (void*)((uint32)position + sizeof(i));

	for ( i = 0; i < data_list.count(); i++)
	{
		mp = (struct message_data*)data_list.get(i);
		size = sizeof(struct message_data) + mp->size;
		memcpy(position, mp, size );
		position = (void*)((uint32)position + size);
	}

//	//printf("%s\n","flattened");

	return ( (uint32)position - (uint32)address );
}

/** The opposite of Flatten, this method takes a flat region of
 * memory and rebuilds this Message's internal data structures.
 * IE: The message is reconstructed.
 *
 * \param address The block of memory to unflatten.
 *
 * \return the size of the memory region which was unflattened.
 */
int32 Message::Unflatten(const char *address)
{
	void *position;
	int32 total;
	int32 i;
	int32 size;
	struct message_data *mp;
	struct message_data *tp;

	position = (void*)address;
	if ( address == NULL ) return -1;

	if ( memcmp( position, "MESSAGE", strlen("MESSAGE") ) != 0 ) 
		return -1;

	position = (void*)((uint32)position + strlen("MESSAGE") );
	memcpy( &what, position, sizeof(what) );
	position = (void*)((uint32)position + sizeof(what));
	memcpy( &total, position, sizeof(total) );
	position = (void*)((uint32)position + sizeof(total));

	for ( i = 0; i < total; i++)
	{
		mp = (struct message_data*)position;
		size = sizeof(struct message_data) + mp->size;
		tp = (struct message_data*)malloc( size );

		if ( tp != NULL )
		{
			memcpy(tp, mp, size );
			data_list.add(tp);
		}
		else
		{
		}
		
		position = (void*)((uint32)position + size);
	}


	return ( (uint32)position - (uint32)address );
}

/** This returns the size in bytes of the memory region required
 * to store the flattened Message object. It's always good to
 * call this object before Flatten()'ing so that you allocate
 * a buffer large enough.
 *
 * \return the size of the buffer required to store flattened Message.
 *
 */
ssize_t Message::FlattenedSize(void) const
{
	int32 i;
	int32 total_size;
	struct message_data *mp;

	total_size = strlen("MESSAGE");
	total_size += sizeof(what);
	total_size += sizeof(i);

	for ( i = 0; i < data_list.count(); i++)
	{
		mp = (struct message_data*)data_list.get(i);
		total_size += sizeof(struct message_data) + mp->size;
	}

	return total_size;
}
	

int32 Message::MakeEmpty(void)
{
	return 0;
}

bool Message::IsEmpty(void) const
{
	return false;
}


void Message::PrintToStream(void) const
{
	struct message_data *mp;
	int32 i;

	//printf("%s(%p)->%p\n","Message",this,this->what);

	for ( i = 0; i < data_list.count(); i++)
	{
		mp = (struct message_data*)data_list.get(i);
		//printf("   %i  %s\n",mp->type,mp->name);
	}
}

/*

	int32 Message::RemoveName(const char *name);
	int32 Message::RemoveData(const char *name, int32 index = 0);
*/
int32 Message::ReplaceData(const char *name, 
		             type_code type, 
		             const void *data, 
			     ssize_t numBytes)
{
  return ReplaceData( name, type, 0, data, numBytes );
}

int32 Message::ReplaceData(const char *name, 
			     type_code type, 
			     int32 index, 
			     const void *data, 
			     ssize_t numBytes)
{
	int i;
	int32 count;
	int32 number;
	struct message_data *md;

	number = 0;

	count = data_list.count();
	for ( i = 0; i < count; i++)
	{
		md = (struct message_data*)data_list.get(i);
		if ( type == md->type )
		  if ( strncmp( name, md->name, MESSAGE_NAME_LENGTH ) == 0 )
		    number++;

	        if ( number > index )
		{
		  // Okay, we have it. Now we replace it.
		    data_list.remove( md );		// remove it
		    free( md );					// free it

		    md = (struct message_data*)
	               malloc( numBytes + sizeof(struct message_data) );
		  
			memcpy( md->name, name, MESSAGE_NAME_LENGTH );
			md->type = type;
			md->size = numBytes;
			memcpy( &(md->data), data, numBytes );
		    
		    data_list.add( md );  // re-insert it.
			/// \todo I took out the add( void*, int pos ) for the new Collection class - 18-07-2005
		    return 0;
		}
	}

	return -1;
}


/*
	int32 Message::ReplaceBool(const char *name, bool aBool);
	int32 Message::ReplaceBool(const char *name, 
			     int32 index, 
			     bool aBool);
	int32 Message::ReplaceInt8(const char *name, int8 anInt8);
	int32 Message::ReplaceInt8(const char *name, 
			     int32 index, 
			     int8 anInt8);
	int32 Message::ReplaceInt16(const char *name, int16 anInt16);
	int32 Message::ReplaceInt16(const char *name, 
			      int32 index, 
			      int16 anInt16);
*/
int32 Message::ReplaceInt32(const char *name, long anInt32)
{
	return ReplaceInt32(name, 0, anInt32 );
}

int32 Message::ReplaceInt32(const char *name, 
			      int32 index, 
			      int32 anInt32)
{
  return ReplaceData( name, INT32_TYPE, index, &anInt32, sizeof(int32) );
}

/*
	int32 Message::ReplaceInt64(const char *name, int64 anInt64);
	int32 Message::ReplaceInt64(const char *name, 
			      int32 index, 
			            int64 anInt64);
	int32 Message::ReplaceFloat(const char *name, float aFloat);
	int32 Message::ReplaceFloat(const char *name, 
			      int32 index, 
			            float aFloat);
	int32 Message::ReplaceDouble(const char *name, double aDouble);
	int32 Message::ReplaceDouble(const char *name, 
			      int32 index, 
			            double aDouble);
	int32 Message::ReplaceString(const char *name, const char *string);
	int32 Message::ReplaceString(const char *name, 
			      int32 index, 
			            const char *string);
	int32 Message::FindString(const char *name, String &string);
	int32 Message::FindString(const char *name, int32 index, String &string);

	int32 Message::ReplaceMessage(const char *name, Message *message);
	int32 Message::ReplaceMessage(const char *name, 
			     int32 index, 
			     Message *message);
	int32 Message::ReplacePointer(const char *name, const void *pointer);
	int32 Message::ReplacePointer(const char *name, 
			     int32 index, 
			     const void *pointer);
*/

Message &Message::operator =(const Message& msg)
{
	int i;
	int count;
	struct message_data *tmp;
	
	this->what = msg.what;

	count = this->data_list.count();
	for ( i = 0; i < count; i++)
	{
	   tmp = (struct message_data*) this->data_list.get( 0 );
	   this->data_list.remove( tmp );
	   free( tmp );
	}

	count = msg.data_list.count();
	for ( i = 0; i < count; i++)
	{
	   tmp = (struct message_data*) msg.data_list.get(i);
	   this->AddData( tmp->name, tmp->type, &(tmp->data), tmp->size );
	}

		// Copy stuff.
	_source_pid   = msg._source_pid;
	_source_port  = msg._source_port;
	_dest_port    = msg._dest_port;
	_replied      = msg._replied;
	


	return *this;
}



// **********************************************************************



int Message::SendReply( uint32 code )
{
  Message *msg = new Message( code );
  
    int ok = Messenger::SendMessage( source_pid(), source_port(), msg );
    if ( ok == 0 ) _replied = true;

  delete msg;
  return ok;
}

int Message::SendReply( uint32 code, Message **reply, int timeout )
{
  Message *msg = new Message( code );
  
   	 *reply = Messenger::SendReceiveMessage( source_pid(), source_port(), msg, timeout );
     _replied = true;

  delete msg;

  if ( reply != NULL ) return 0;
  return -1;
}


int Message::SendReply( Message *msg )
{
     int ok = Messenger::SendMessage( source_pid(), source_port(), msg );
     if ( ok == 0 ) _replied = true;
  return ok;
}

int Message::SendReply( Message *msg, Message **reply, int timeout )
{
     *reply = Messenger::SendReceiveMessage( source_pid(), source_port(), msg, timeout );
     _replied = true;
  if ( reply != NULL ) return 0;
  return -1;
}
	

