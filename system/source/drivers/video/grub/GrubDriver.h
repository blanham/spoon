#ifndef _GRUB_DRIVER_H
#define _GRUB_DRIVER_H

#include <spoon/video/VideoDriver.h>

class GrubDriver : public VideoDriver
{
  public:
	GrubDriver();
	~GrubDriver();
	
	  virtual int   setMode( int width, int height, int depth );
	  virtual void* getLFB( int *width, int *height, int *depth );
      virtual int	init();
      virtual int	shutdown();
 
	  virtual int	sync( int x, int y, int width, int height );

   private:

	  void Indicate(int num);
	  
	  
	  uint32 m_height;
	  uint32 m_width;
	  uint32 m_depth;
	  void *m_lfb;

};


#endif

