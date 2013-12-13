#ifndef _SPOON_INTERFACE_FONT_H
#define _SPOON_INTERFACE_FONT_H

#include <types.h>
#include <spoon/support/String.h>

/** \ingroup interface 
 *
 */
class Font
{
	public:
		Font( const char *fontName, const char fontFamily ); 

		int StringWidth(const char *string) const;
		int StringWidth(const char *string, int32 length) const;
	
		String	FontName();
		String	FontFamily();
		int		FontSize();
		unsigned int	FontFace();

		void	SetFace( unsigned int face );
		
	private:
		String 	_fontName;
		String 	_fontFamily;
		int		_fontSize;
		unsigned int	_fontFace;
			
};

#endif

