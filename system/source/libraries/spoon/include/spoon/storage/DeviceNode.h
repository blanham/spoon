#ifndef _SPOON_STORAGE_DEVICENODE_H
#define _SPOON_STORAGE_DEVICENODE_H

#include <types.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Message.h>
#include <spoon/storage/StorageDefs.h>

/** \ingroup storage
 *
 *
 */
class DeviceNode : public Looper
{
   public:
      DeviceNode( char *node );
      ~DeviceNode();

    public:
      char *GetNode() { return _node; }


    public:
       virtual void Install();
       virtual void Uninstall();


       virtual int loaded() { return -1; }
       virtual int eject() { return -1; }
       
       virtual int read( int64 pos, void *buffer, int len );
       virtual int write( int64 pos, void *buffer, int len );

       virtual int stat( struct stat *st );
	   
       virtual int packet( int cmdSize, void *cmd, int dir,
       			   long bufSize, void* buffer,
			   unsigned long lba );



    public:
       int Run()
       {
         Install();
		 Looper::Run();
		 Uninstall();
		 return 0;
       }
     
       
       void MessageReceived( Message *msg );

   private:
      char *_node;

   
};

#endif


