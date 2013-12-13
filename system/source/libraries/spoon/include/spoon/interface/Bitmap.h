#ifndef _SPOON_INTERFACE_BITMAP_H
#define _SPOON_INTERFACE_BITMAP_H

#include <types.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Drawing.h>

#define BITMAP_CROP			1
#define BITMAP_SCALE		2


/** \ingroup interface 
 *
 */
class Bitmap
{
	public:
		Bitmap();
		Bitmap( int width, int height, void *data = NULL );
		~Bitmap();


		bool resize( int width, int height, int mode = BITMAP_SCALE );

		const Rect& Frame();
		uint32* image_data();


		void SetBackgroundColour( uint32 col );
		void PutPixel( int x, int y, uint32 col ) ;
		uint32 GetPixel( int x, int y );
		
	private:
		uint32 *m_data;
		int m_width;
		int m_height;
		Rect m_rect;
		uint32 m_bgCol;


		bool resize_crop( int width, int height, uint32 *new_data );
		bool resize_scale( int width, int height, uint32 *new_data );
};

#endif

