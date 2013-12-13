#ifndef _SPOON_INTERFACE_TEXTINPUT_H
#define _SPOON_INTERFACE_TEXTINPUT_H


#include <types.h>
#include <spoon/support/String.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/View.h>



/** \ingroup interface 
 *
 */
class TextInput : public View
{
   public:
      TextInput( Rect frame, uint32 follows = FOLLOW_ALL_SIDES );
      virtual ~TextInput();

	const char *Text();
	void SetText( const char *text );


	virtual void Draw( Rect area );

	virtual void KeyDown( uint32 code, uint32 ascii, uint32 modifiers );
	virtual void KeyUp( uint32 code, uint32 ascii, uint32 modifiers );

   private:
	  char *m_text;
};

#endif


