#ifndef _DESKBAR_MEMVIEW_H
#define _DESKBAR_MEMVIEW_H


#include <types.h>
#include <spoon/interface/View.h>

class MemView : public View
{
	public:
		MemView( Rect frame );
		~MemView();

		void Draw( Rect frame );
		virtual void Pulsed();

	private:
		Bitmap *m_background;
};


#endif

