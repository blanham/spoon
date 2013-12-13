#ifndef _THREADED_LIST_H
#define _THREADED_LIST_H

#include <spoon/base/Thread.h>

class ThreadedList : public Thread
{
   public:
      ThreadedList( char *name );
      int Run();
};

#endif

