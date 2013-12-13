#ifndef _SPOON_VIDEO_GUI_CONTROL_H_
#define _SPOON_VIDEO_GUI_CONTROL_H_

#include <spoon/collections/List.h>

struct video_mode
{
	int id;
	int width;
	int height;
	int mode;
};


struct desktop_info
{
	int id;
	int video_driver;
	char name[1024];
	char description[1024];
};

struct video_driver_info
{
	int id;
	char name[1024];
	char description[1024];
};

// ----

/** \ingroup video
 *
 */
class GUIControl 
{
	public:
		GUIControl();
		virtual ~GUIControl();

		int CreateDesktop( char *name, char *description, int *id );
		int DeleteDesktop( int desktop_id );
		

		int GetDriverList( List *drivers );
		int GetDesktopList( List *desktops );
		int GetSupportedModes( List* modes );
		int GetDesktop( int *desktop_id );
		
		int GetDesktopMode( int desktop_id, 
							int *video_id, 
							int *width, int *height, int *mode );

		
		
		int SetDesktopDriver( int desktop_id, int video_id );
		int SetMode( int video_id, int width, int height, int mode );
		int SetDesktop( int desktop_id );


};


#endif


