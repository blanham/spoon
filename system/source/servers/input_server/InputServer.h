#ifndef _INPUT_SERVER_H
#define _INPUT_SERVER_H

#include <spoon/ipc/Messenger.h>
#include <spoon/app/Application.h>

class Messenger;

class InputServer : public Application
{
   public: 
     InputServer();
     void PulseReceived( Pulse *p );


   private:
     bool guiMode;
     int _gui_pid;

};

#endif

