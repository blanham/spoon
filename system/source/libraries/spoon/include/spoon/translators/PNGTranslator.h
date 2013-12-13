#ifndef _PNGLOADER_H
#define _PNGLOADER_H

#include <spoon/interface/Bitmap.h>
#include <spoon/translators/ImageTranslator.h>

#include <png.h>

typedef unsigned char   uch;
typedef unsigned short  ush;
typedef unsigned long   ulg;

#define alpha_composite(composite, fg, alpha, bg) {               \
    ush temp = ((ush)(fg)*(ush)(alpha) +                          \
                (ush)(bg)*(ush)(255 - (ush)(alpha)) + (ush)128);  \
    (composite) = (uch)((temp + (temp >> 8)) >> 8);               \
}


#define PNG_USEFILE_BG			1


/** \ingroup translators
 *
 */
class PNGTranslator : public ImageTranslator
{
	public:
		PNGTranslator();
		~PNGTranslator();
			
		virtual bool test( const char *filename );
		virtual Bitmap* load( const char *filename );
		virtual int save( const char *filename, Bitmap* bitmap );



	private:
		int reset( bool total );
		int load_data( const char *filename, 
					   unsigned int bgCol = PNG_USEFILE_BG, 
					   const char *gamma = NULL );
		int decode_data( Bitmap *bitmap );
		
		// File operationg
		int init(FILE *infile, ulg *pWidth, ulg *pHeight);
		int get_bgcolor(uch *red, uch *green, uch *blue);
		uch *get_image(double display_exponent, 
					   int *pChannels, ulg *pRowbytes);
		

		//
		
		png_structp png_ptr;
		png_infop info_ptr;

		png_uint_32  width, height;
		int  bit_depth, color_type;
		uch  *image_data;


		uch bg_red, bg_green, bg_blue;
		double display_exponent;


		ulg image_width, image_height, image_rowbytes;
		int image_channels;
	
};


#endif

