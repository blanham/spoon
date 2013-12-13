#ifndef _SPOON_VIDEO_VIDEODRIVER_H
#define _SPOON_VIDEO_VIDEODRIVER_H

#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/ipc/Looper.h>
#include <spoon/ipc/Pulse.h>
#include <spoon/ipc/Message.h>


/** \ingroup video
 *
 */
class VideoDriver : public Looper
{
   public:
      VideoDriver( char *name, char *description, bool receives_syncs );
      virtual ~VideoDriver();

	  
	  int addMode( int width, int height, int depth );

	  virtual int   setMode( int width, int height, int depth ) = 0;
	  virtual void* getLFB( int *width, int *height, int *depth ) = 0;
      virtual int	init() = 0;
      virtual int	shutdown() = 0;

	  virtual int	sync( int x, int y, int width, int height ) = 0;
       
	  virtual void MessageReceived( Message *msg );
      virtual void PulseReceived( Pulse *pulse );
       
	  
	  int Run();

   private:
	  int get_lfb( Message *msg );
	  
	  int get_message( Message *msg );

	  
      int install();
      int uninstall();
	   
	  List _modes;
   
	  char *_name;
	  char *_description;
	  bool m_sync;

	  int _id;
};

#endif


