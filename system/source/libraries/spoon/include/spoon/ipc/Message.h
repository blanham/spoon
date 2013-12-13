#ifndef _SPOON_IPC_MESSAGE_H
#define _SPOON_IPC_MESSAGE_H

#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/support/String.h>
#include <spoon/interface/Rect.h>

class Looper;

#define MESSAGE_NAME_LENGTH	64

#define		INFINITE_TIMEOUT	0

#define		TIMEOUT_1SEC		(1000)
#define		TIMEOUT_10SEC		(10000)
#define		TIMEOUT_15SEC		(115000)
#define		TIMEOUT_20SEC		(20000)
#define		TIMEOUT_30SEC		(30000)
#define		TIMEOUT_45SEC		(45000)
#define		TIMEOUT_1MIN		(60000)
#define		TIMEOUT_5MIN		(5*60000)
#define		TIMEOUT_10MIN		(10*60000)
#define		TIMEOUT_30MIN		(30*60000)

#define		TIMEOUT_DEFAULT		(TIMEOUT_1MIN)



typedef enum {
		CHAR_TYPE, 
		INT8_TYPE, 
		INT16_TYPE, 
		INT32_TYPE,  
		INT64_TYPE, 
		UINT8_TYPE, 
		UINT16_TYPE, 
		UINT32_TYPE, 
		UINT64_TYPE, 
		FLOAT_TYPE, 
		DOUBLE_TYPE, 
		BOOL_TYPE, 
		POINTER_TYPE, 
		MESSAGE_TYPE, 
		STRING_TYPE, 
		RECT_TYPE,
		RAW_TYPE, 
		ANY_TYPE
	     }  type_code;

#define INT_TYPE	INT32_TYPE


/**  \ingroup ipc
 *
 * The Message class abstracts a system IPC method which sends
 * large chunks of memory between processes and ports. Several
 * methods are implemented to make adding and removing from
 * the Message easier. It's quite a useful class.
 *
 * A Looper class will receive a block of memory via a recv_msg
 * system call and will call the Message::Unflatten() method to
 * unflatten the memory into a Message object.
 * This object is passed to the MessageReceived() method of the
 * Looper.
 *
 */
class Message 
{
 
	public:
	  Message(uint32 command);
	  Message(const Message &message);
	  Message(void);

	  virtual ~Message();


	// ****** COMMUNICATION ******************

       public:
	  
		int SendRC( int rc, char *error = NULL );
	  
        int SendReply( uint32 code );
        int SendReply( uint32 code, Message **reply, 
				int timeout = TIMEOUT_DEFAULT );
		int SendReply( Message *msg );
		int SendReply( Message *msg, Message **reply,
				int timeout = TIMEOUT_DEFAULT );
	
	
		int source_pid()        { return _source_pid; }
		int source_port()       { return _source_port; }
		int dest_port()         { return _dest_port; }

		int rc();
		char *error();

        bool Replied() { return _replied; }

       private:
         friend class Looper;
	     friend class Messenger;
		 friend class Application;

         int _source_pid;	///< The pid of the originating process.
		 int _source_port;  ///< The originating port of the message.
		 int _dest_port;    ///< The destination port of the message.

		 bool _replied;		///< A flag to indicate if the message has sent a reply.
	
	// ******** END COMMUNICATION ************


       public:
	int32 AddData(const char *name, type_code type, 
		         const void *data, 
		         ssize_t numBytes, 
		         bool fixedSize = true, 
		         int32 numItems = 1);
	int32 AddBool(const char *name, bool aBool);
	int32 AddInt(const char *name, int anInt);
	int32 AddInt8(const char *name, int8 anInt8);
	int32 AddInt16(const char *name, int16 anInt16);
	int32 AddInt32(const char *name, int32 anInt32); 
	int32 AddInt64(const char *name, int64 anInt64); 
	int32 AddFloat(const char *name, float aFloat);
	int32 AddDouble(const char *name, double aDouble);
	int32 AddString(const char *name, const char *string);
	int32 AddString(const char *name, const String &string);
	int32 AddRect(const char *name, Rect rect );
	int32 AddMessage(const char *name, const Message *message);
	int32 AddPointer(const char *name, const void *pointer);
	  
	int32 CountNames(type_code type) const;

	int32 FindData(const char *name, 
	 	          type_code type, 
		          int32 index, 
		          const void **data, 
		          ssize_t *numBytes) const;
	int32 FindData(const char *name, 
		          type_code type, 
		          const void **data, 
		          ssize_t *numBytes) const;
	int32 FindBool(const char *name, 
			      int32 index, 
			            bool *aBool) const;
	int32 FindBool(const char *name, bool *aBool) const;
	int32 FindInt(const char *name, 
			      int32 index, 
		          int *anInt) const;
	int32 FindInt(const char *name, 
			      int *anInt) const;
	int32 FindInt8(const char *name, 
			      int32 index, 
			            int8 *anInt8) const;
	int32 FindInt8(const char *name, 
			      int8 *anInt8) const;
	int32 FindInt16(const char *name, 
			      int32 index, 
			            int16 *anInt16) const;
	int32 FindInt16(const char *name, int16 *anInt16) const;
	int32 FindInt32(const char *name, 
			      int32 index, 
			            int32 *anInt32) const;
	int32 FindInt32(const char *name, int32 *anInt32) const;
	int32 FindInt64(const char *name, 
			      int32 index, 
			            int64 *anInt64) const;
	int32 FindInt64(const char *name, int64 *anInt64) const;
	int32 FindFloat(const char *name, 
			      int32 index, 
			            float *aFloat) const;
	int32 FindFloat(const char *name, float *aFloat) const;
	int32 FindDouble(const char *name, 
			      int32 index, 
			            double *aDouble) const;
	int32 FindDouble(const char *name, double *aDouble) const;
	int32 FindString(const char *name, 
			      int32 index, 
			            const char **string) const;
	int32 FindString(const char *name, const char **string) const;
	int32 FindString(const char *name, String *string) const;
	int32 FindString(const char *name, int32 index, String *string) const;
	int32 FindRect(const char *name, Rect *rect) const;
	int32 FindRect(const char *name, int32 index, Rect *rect) const;
	int32 FindMessage(const char *name, 
			      int32 index, 
			            Message *message) const;
	int32 FindMessage(const char *name, Message *message) const;
	int32 FindPointer(const char *name, 
			      int32 index, 
			            void **pointer) const;
	int32 FindPointer(const char *name, void **pointer) const;
	

	int32 Flatten(char *address, ssize_t numBytes = NULL) const;
	int32 Unflatten(const char *address);
	ssize_t FlattenedSize(void) const;
	

	int32 MakeEmpty(void);
	bool IsEmpty(void) const;


	void PrintToStream(void) const;

	int32 RemoveName(const char *name);
	int32 RemoveData(const char *name, int32 index = 0);

	int32 ReplaceData(const char *name, 
		             type_code type, 
		             const void *data, 
			     ssize_t numBytes);
	int32 ReplaceData(const char *name, 
			     type_code type, 
			     int32 index, 
			     const void *data, 
			     ssize_t numBytes);
	int32 ReplaceBool(const char *name, bool aBool);
	int32 ReplaceBool(const char *name, 
			     int32 index, 
			     bool aBool);
	int32 ReplaceInt(const char *name, int anInt);
	int32 ReplaceInt(const char *name, 
			     int32 index, 
			     int anInt);
	int32 ReplaceInt8(const char *name, int8 anInt8);
	int32 ReplaceInt8(const char *name, 
			     int32 index, 
			     int8 anInt8);
	int32 ReplaceInt16(const char *name, int16 anInt16);
	int32 ReplaceInt16(const char *name, 
			      int32 index, 
			      int16 anInt16);
	int32 ReplaceInt32(const char *name, long anInt32);
	int32 ReplaceInt32(const char *name, 
			      int32 index, 
			      int32 anInt32);
	int32 ReplaceInt64(const char *name, int64 anInt64);
	int32 ReplaceInt64(const char *name, 
			      int32 index, 
			            int64 anInt64);
	int32 ReplaceFloat(const char *name, float aFloat);
	int32 ReplaceFloat(const char *name, 
			      int32 index, 
			            float aFloat);
	int32 ReplaceDouble(const char *name, double aDouble);
	int32 ReplaceDouble(const char *name, 
			      int32 index, 
			            double aDouble);
	int32 ReplaceString(const char *name, const char *string);
	int32 ReplaceString(const char *name, 
			      int32 index, 
			            const char *string);
	int32 FindString(const char *name, String &string);
	int32 FindString(const char *name, int32 index, String &string);
	int32 ReplaceMessage(const char *name, Message *message);
	int32 ReplaceMessage(const char *name, 
			     int32 index, 
			     Message *message);
	int32 ReplacePointer(const char *name, const void *pointer);
	int32 ReplacePointer(const char *name, 
			     int32 index, 
			     const void *pointer);
	

	Message &operator =(const Message&);


       public:
	        uint32 what; 		///< A uint32 value which is the main "value" of the message. 


	private:
	 	List data_list; 		///< The Collection used by the Message to store it's data.

};

#endif


