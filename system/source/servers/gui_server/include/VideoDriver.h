#ifndef _GUI_SERVER_VIDEODRIVER_H
#define _GUI_SERVER_VIDEODRIVER_H


#include <spoon/collections/List.h>

namespace GUI
{

	class VideoDriver 
	{
		public:
	        ~VideoDriver();

			VideoDriver* create( int pid, int port,
								  char *name,
								  char *description,
								  bool sync,
								  List *modes,
								  int *id );

			int id();


			bool getMode( int num, int *width, int *height, int *mode );
			bool hasMode( int width, int height, int mode );
			bool setMode( int width, int height, int mode );

			
	    private:
	        VideoDriver();


			int m_id;
			int m_pid;
			int m_port;
			bool m_sync;
			char *m_name;
			char *m_description;


			int m_sid;
			void *m_LFB;

			List m_supported_modes;

	};



}

#endif

