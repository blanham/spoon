#include <stdlib.h>
#include <string.h>
#include <spoon/translators/ImageTranslator.h>


ImageTranslator::ImageTranslator( const char *name, 
						 		  const char *description )
{
	m_name = strdup( name );
	m_description = strdup( description );
}

ImageTranslator::~ImageTranslator()
{
	if ( m_name != NULL ) free( m_name );
	if ( m_description != NULL ) free( m_description );
}



const char *ImageTranslator::name()
{
	return m_name;
}

const char *ImageTranslator::description()
{
	return m_description;
}




