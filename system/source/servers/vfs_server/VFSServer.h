#ifndef _VFS_SERVER_H
#define _VFS_SERVER_H

#include <spoon/ipc/Messenger.h>
#include <spoon/app/Application.h>

#include "primitives/EntityTable.h"
#include "io/DeviceTable.h"
#include "tables/FileTable.h"
#include "tables/MountTable.h"
//#include "EnvTable.h"

class Messenger;



extern FileTable file_table;
extern MountTable mount_table;
extern VFS::DeviceTable device_table;
extern VFS::EntityTable entity_table;


class VFSServer : public Application
{
   public: 
     VFSServer();



     void installDevice( char *node, int pid, int port );
     void uninstallDevice( int pid, int port );


     void doMount( Message *msg );
     void doUnmount( Message *msg );

     
	 void createFile( Message *msg, int pid, char *node );
	 void deleteFile( Message *msg, int pid, char *node );

     void openFile( Message *msg, int pid, char *node );
     void closeFile( Message *msg, int pid, int fd );

     void readFile( Message *msg,  int pid, int fd );
     void writeFile( Message *msg, int pid, int fd );
     void seekFile( Message *msg, int pid, int fd );
     void statFile( Message *msg, int pid, int fd );

     void ejectFile( Message *msg, int pid, int fd );
     void loadedFile( Message *msg, int pid, int fd );


     void listRequest( Message *msg );
     void listDevices( Message *msg, char *path );

     void makeDirectory( Message *msg );
     void deleteDirectory( Message *msg );

	 
   private:
     char *fixNode( int pid, char *node, bool endingSlash,
     		    bool clean = true);

   public:
     void MessageReceived( Message *msg );
     void PulseReceived( Pulse *p );



};

#endif

