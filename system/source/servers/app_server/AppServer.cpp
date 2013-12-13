#include <types.h>
#include <kernel.h>
#include <string.h>
#include <stdio.h>
#include <spoon/sysmsg.h>
#include "AppServer.h"

#include "ThreadedList.h"

// **********************************************

AppServer::AppServer()
: Application("app_server")
{
   ThreadedList *bob = new ThreadedList( "bobby" );

                 bob->Resume();
}


// *********************************************


