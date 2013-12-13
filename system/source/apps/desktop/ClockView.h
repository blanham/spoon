#ifndef _DESKTOP_CLOCKVIEW_H
#define _DESKTOP_CLOCKVIEW_H


#include <types.h>
#include <spoon/interface/View.h>

class ClockView : public View
{
	public:
		ClockView( Rect frame );
		virtual ~ClockView();

		void setDateFormat( const char *format );
		void Draw( Rect frame );

		void Pulsed();
		
	private:
		char *m_date_format;
		Bitmap *m_background;
};


#endif

