#ifndef _DOTVIEW_H
#define _DOTVIEW_H

#include <spoon/interface/View.h>

class DotView : public View
{
	public:
		DotView( Rect frame )
		: View( frame )
		{
		}

		virtual void Draw( Rect frame )
		{
			int middle 	= Frame().Width() / 2;
			int dots 	= Frame().Height() / 7;

			for ( int i = 0; i < dots; i++ )
			{
				int y = i * 7;

				PutPixel( middle, y + 3, 0x404040 );
				PutPixel( middle, y + 6, 0x404040 );
				PutPixel( middle + 1, y + 3, 0x404040 );
				PutPixel( middle + 1, y + 6, 0x404040 );

				PutPixel( middle + 2, y + 4, 0x404040 );
				PutPixel( middle + 2, y + 5, 0x404040 );
				PutPixel( middle - 1, y + 4, 0x404040 );
				PutPixel( middle - 1, y + 5, 0x404040 );
			}
		}

		
};


#endif

