#ifndef _TEXTUAL_DRIVER_H
#define _TEXTUAL_DRIVER_H

#include <spoon/video/VideoDriver.h>


class TextualDriver : public VideoDriver
{
  public:
	TextualDriver();
	~TextualDriver();
	
	  virtual int   setMode( int width, int height, int depth );
	  virtual void* getLFB( int *width, int *height, int *depth );
      virtual int	init();
      virtual int	shutdown();

	  virtual int	sync( int x, int y, int width, int height );
 

  private:
	  void Fill();
	  void Thunk( char *screen, unsigned int* lfb, int width, int height );

	  
	  char *m_screen;
	  unsigned int* m_lfb;
	  int m_width;
	  int m_height;
	  int m_depth;
	  

};


#endif
