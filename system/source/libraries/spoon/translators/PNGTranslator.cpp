#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>


#include <spoon/translators/PNGTranslator.h>


// ************ PNG READ OPERATION OVERRIDE **************
// External stuff for overriding the default PNG read function.

char *file_data = NULL;
unsigned int file_pos = 0;
unsigned int file_size = 0;

void my_png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	int to_read = length;
	
	// sort it out, if we were asked for too much.
	if ( (file_pos + to_read) > file_size ) to_read = file_size - file_pos;

	if ( to_read < 0 ) png_error( png_ptr, "Read Error" );

	memcpy( data, (const char*)&(file_data[file_pos]), to_read );
	file_pos += to_read;
}

// ************ PNG READ OPERATION OVERRIDE FINISHED *****


PNGTranslator::PNGTranslator()
: ImageTranslator( "image/png", "PNG support through libpng" )
{
	image_data = NULL;
	png_ptr = NULL;
	info_ptr = NULL;
	bg_red = 0;
	bg_green = 0; 
	bg_blue = 0;
}


PNGTranslator::~PNGTranslator()
{
	reset( true );
}


// ---------------------------------------------------------

bool PNGTranslator::test( const char *filename )
{
	int rc;
	FILE *infile = NULL;

    if (!(infile = fopen(filename, "rb"))) return false;
	
		// Attempt to load the data.
        if ((rc = init(infile, &image_width, &image_height)) != 0) 
		{
			reset( true );
            fclose(infile);
			return false;
		}
		
	reset( true );
	fclose( infile );
	return true;
}

Bitmap* PNGTranslator::load( const char *filename )
{
	reset( true );

	int rc = PNGTranslator::load_data( filename );
	if ( rc != 0 ) 
	{
		reset( true );
		return NULL;	// Failure.
	}

	Bitmap *result = new Bitmap( image_width, image_height );
	
	if ( decode_data( result ) != 0 )
	{
		reset( true );
		delete result;
		return NULL;
	}
	
	reset( true );
	return result;
}

int PNGTranslator::save( const char *filename, Bitmap* bitmap )
{
	return -1;	// Easy.
}


// ---------------------------------------------------------


int PNGTranslator::reset( bool total )
{
	if ( total == true )
	{
		if ( image_data != NULL ) free( image_data );
		image_data = NULL;
	}

	if ( file_data != NULL ) 
	{
		free( file_data );
		file_data = NULL;
	}

    if ( (png_ptr != NULL) && (info_ptr != NULL) ) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }

	bg_red = 0;
	bg_green = 0; 
	bg_blue = 0;
	
	return 0;
}

int PNGTranslator::decode_data( Bitmap* bitmap )
{
    uch *src;
    uch r, g, b, a;
    ulg i, row;
    ulg pixel;


        ulg red, green, blue;

        for (row = 0;  row < image_height;  ++row) {
            src = image_data + row*image_rowbytes;

            if (image_channels == 3) {
                for (i = image_width;  i > 0;  --i) {
                    red   = *src++;
                    green = *src++;
                    blue  = *src++;

                    pixel = (red   << 16) |
                            (green << 8) |
                            (blue);

					// Put the pixel into the bitmap
					bitmap->PutPixel( image_width - i, row, pixel );
					
                }
            } else /* if (image_channels == 4) */ {
                for (i = image_width;  i > 0;  --i) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
                    if (a == 255) {
                        red   = r;
                        green = g;
                        blue  = b;
                    } else if (a == 0) {
                        red   = bg_red;
                        green = bg_green;
                        blue  = bg_blue;
                    } else {
                        /* this macro (from png.h) composites the foreground
                         * and background values and puts the result into the
                         * first argument */
                        alpha_composite(red,   r, a, bg_red);
                        alpha_composite(green, g, a, bg_green);
                        alpha_composite(blue,  b, a, bg_blue);
                    }
                    pixel = (red   << 16) |
                            (green << 8) |
                            (blue  << 0);

					// Put the pixel into the bitmap
					bitmap->PutPixel( image_width - i, row, pixel );
                }
            }

        }

    return 0;
}


int PNGTranslator::load_data( const char *filename, 
						   unsigned int bgCol,
						   const char *gamma )
{
	int rc;
    double LUT_exponent;               /* just the lookup table */
    double CRT_exponent = 2.2;         /* just the monitor */
    double default_display_exponent;   /* whole display system */



    LUT_exponent = 1.0;   /* assume no LUT:  most PCs */

    default_display_exponent = LUT_exponent * CRT_exponent;


	if ( gamma != NULL ) display_exponent = atof(gamma);
				    else display_exponent = default_display_exponent;


	FILE *infile = NULL;

    if (!(infile = fopen(filename, "rb"))) 
	{
		printf("%s\n","unable to open file.");
		return -1;
	}
	
		// Attempt to load the data.
        if ((rc = init(infile, &image_width, &image_height)) != 0) 
		{
            fclose(infile);
			printf("%s\n","unable to init.");
			return rc;
		}
		

	if ( bgCol != PNG_USEFILE_BG )
	{
	     bg_red   = (uch)((bgCol >> 16) & 0xFF );
	     bg_green = (uch)((bgCol >> 8) & 0xFF);
	     bg_blue  = (uch)((bgCol & 0xFF));
	} 
	else if (get_bgcolor(&bg_red, &bg_green, &bg_blue) > 1) 
		{
			reset( true );
			printf("%s\n","unable to get_bgcolor.");
			return -2;
	    }



    image_data = get_image(display_exponent, 
							&image_channels,
					        &image_rowbytes);

	reset( false );
    fclose(infile);

	// Unablet to read the image data.
    if ( image_data == NULL ) 
	{
		printf("%s\n","image_data was null");
		return -3;
	}

    return 0;
}





int PNGTranslator::init(FILE *infile, ulg *pWidth, ulg *pHeight)
{
    uch sig[8];
	int fd;
	struct stat st;


    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
	{
		printf("%s\n","bad signature.");
        return 1;   /* bad signature */
	}


    /* could pass pointers to user-defined error handlers instead of NULLs: */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }

	/* DURAND - we desparately need to override the default png read
	 * function because the library is only reading 4 bytes at a time.
	 * This is stupid. */
	
	// Read everything into memory.
	
		fd = infile->fd;
	
		if ( fstat( fd, &st ) != 0 )
		{
	        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return 2;
		}
	
		if ( fseek( infile, 0, SEEK_SET ) != 0 )
		{
	        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return 2;
		}
	
		file_data = (char*)malloc( st.st_size );
		if ( file_data == NULL )
		{
	        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return 4;
		}
	
		if ( read( fd, file_data, st.st_size ) != st.st_size ) 
		{
	        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return 2;
		}
	
	// Go back past the signature.
	if ( fseek( infile, 8, SEEK_SET ) != 0 )
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 2;
	}

	file_pos = 8;
	file_size = st.st_size;
	
	
    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }


    png_init_io(png_ptr, infile);

	// DURAND - override the default read.
	png_set_read_fn( png_ptr, infile, my_png_read );
	
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */


    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL);
    *pWidth = width;
    *pHeight = height;

    /* OK, that's all we need for now; return happy */

    return 0;
}



/* returns 0 if succeeds, 1 if fails due to no bKGD chunk, 2 if libpng error;
 * scales values to 8-bit if necessary */

int PNGTranslator::get_bgcolor(uch *red, uch *green, uch *blue)
{
    png_color_16p pBackground;


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }


    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
        return 1;

    /* it is not obvious from the libpng documentation, but this function
     * takes a pointer to a pointer, and it always returns valid red, green
     * and blue values, regardless of color_type: */

    png_get_bKGD(png_ptr, info_ptr, &pBackground);


    /* however, it always returns the raw bKGD data, regardless of any
     * bit-depth transformations, so check depth and adjust if necessary */

    if (bit_depth == 16) {
        *red   = pBackground->red   >> 8;
        *green = pBackground->green >> 8;
        *blue  = pBackground->blue  >> 8;
    } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        if (bit_depth == 1)
            *red = *green = *blue = pBackground->gray? 255 : 0;
        else if (bit_depth == 2)
            *red = *green = *blue = (255/3) * pBackground->gray;
        else /* bit_depth == 4 */
            *red = *green = *blue = (255/15) * pBackground->gray;
    } else {
        *red   = (uch)pBackground->red;
        *green = (uch)pBackground->green;
        *blue  = (uch)pBackground->blue;
    }

    return 0;
}

/* display_exponent == LUT_exponent * CRT_exponent */

uch *PNGTranslator::get_image(double display_exponent, int *pChannels, ulg *pRowbytes)
{
    double  gamma;
    png_uint_32  i, rowbytes;
    png_bytepp  row_pointers = NULL;


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }


    /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);


    /* unlike the example in the libpng documentation, we have *no* idea where
     * this file may have come from--so if it doesn't have a file gamma, don't
     * do any correction ("do no harm") */

    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, display_exponent, gamma);


    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *pChannels = (int)png_get_channels(png_ptr, info_ptr);

    if ((image_data = (uch *)malloc(rowbytes*height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return NULL;
    }



    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0;  i < height;  ++i)
        row_pointers[i] = image_data + i*rowbytes;


    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);


    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    free(row_pointers);
    row_pointers = NULL;

    png_read_end(png_ptr, NULL);

    return image_data;
}


