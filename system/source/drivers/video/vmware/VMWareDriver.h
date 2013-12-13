#ifndef _VMWARE_DRIVER_H
#define _VMWARE_DRIVER_H

#include <spoon/video/VideoDriver.h>


class VMWareDriver : public VideoDriver
{
  public:
	VMWareDriver( unsigned int index, unsigned int value );
	~VMWareDriver();
	
	  virtual int   setMode( int width, int height, int depth );
	  virtual void* getLFB( int *width, int *height, int *depth );
      virtual int	init();
      virtual int	shutdown();
 
	  virtual int	sync( int x, int y, int width, int height );


	private:

	  void OUT( int port, unsigned int value );
	  int IN( int port );
	  
	  unsigned int index_reg;
	  unsigned int value_reg;
};


#endif

