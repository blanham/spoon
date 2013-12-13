#ifndef _SPOON_TRANSLATORS_IMAGETRANSLATOR_H
#define _SPOON_TRANSLATORS_IMAGETRANSLATOR_H

#include <spoon/interface/Bitmap.h>

/** \ingroup translators 
 *
 * ImageTranslator is the base class from which all
 * image translators should be derived. The job of an
 * ImageTranslator is to load an image file - in
 * whatever format it is - and return a Bitmap object
 * to the system. It's also supposed to be able to
 * save a Bitmap in the format that the ImageTranslator
 * implements.
 *
 * The ImageTranslator may or may not interact with the
 * user in order to determine certain information 
 * particular to the image format being used.
 */

class ImageTranslator
{
	public:
		ImageTranslator( const char *name, 
						 const char *description );
		virtual ~ImageTranslator();

		const char *name();
		const char *description();

		virtual bool test( const char *filename ) = 0;
		virtual Bitmap* load( const char *filename ) = 0;
		virtual int save( const char *filename, Bitmap* bitmap ) = 0;


	private:
		char *m_name;
		char *m_description;


};


#endif

