#include <spoon/storage/VFSControl.h>
#include <spoon/ipc/Messenger.h>
#include <spoon/sysmsg.h>

VFSControl::VFSControl()
{
}

VFSControl::~VFSControl()
{
}

int VFSControl::Mount( char *device, char *point, char *fs )
{
  Message *msg = new Message(MOUNT);

    msg->AddString("point", point );
    msg->AddString("node", device );
    msg->AddString("fs",fs);
    
    Message *reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
    delete msg;
    if ( reply == NULL ) return -1;

    int ans = -1;
    if ( reply->what == OK ) ans = 0;
    delete reply;
    return ans;
}

int VFSControl::Unmount( char *point )
{
  Message *msg = new Message(UNMOUNT);

    msg->AddString("point", point );
    
    Message *reply = Messenger::SendReceiveMessage( "vfs_server", 0, msg );
    delete msg;
    if ( reply == NULL ) return -1;

    int ans = -1;
    if ( reply->what == OK ) ans = 0;
    delete reply;
    return ans;
}


