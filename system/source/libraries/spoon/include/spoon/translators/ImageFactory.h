#ifndef _SPOON_TRANSLATORS_IMAGEFACTORY_H
#define _SPOON_TRANSLATORS_IMAGEFACTORY_H

#include <spoon/collections/List.h>
#include <spoon/interface/Bitmap.h>
#include <spoon/translators/ImageTranslator.h>

/** \ingroup translators
 *
 */
class ImageFactory : public List
{
	public:
		ImageFactory();
		~ImageFactory();

		ImageTranslator* test( const char *filename );
		Bitmap* load( const char *filename );
};

extern ImageFactory imageFactory;

#endif

