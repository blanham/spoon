#ifndef _SPOON_INTERFACE_DRAWING_H
#define _SPOON_INTERFACE_DRAWING_H

#include <types.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>

class Bitmap;


/** \ingroup interface 
 *
 */
class Drawing
{
   public:
      Drawing();

    virtual void PutPixel( int x, int y, uint32 col ) = 0;
    virtual uint32 GetPixel( int x, int y ) = 0;

    virtual void FillRect( Rect block, uint32 col );
    virtual void FillRect( int x1, int y1, int x2, int y2, uint32 col );
    virtual void DrawRect( Rect block, uint32 col );
    virtual void DrawRect( int x1, int y1, int x2, int y2, uint32 col );
    virtual void DrawLine( int x1, int y1, int x2, int y2, uint32 col );
    virtual void DrawString( int x, int y, char *string, uint32 col );
    virtual void DrawString( int x, int y, char *string, uint32 col, uint32 bg );

	virtual void DrawBitmap( int x, int y, Bitmap* bitmap );
	virtual void DrawBitmap( int x, int y, int width, int height,
							Bitmap* bitmap );

};

#endif

