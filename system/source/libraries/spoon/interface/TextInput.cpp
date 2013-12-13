#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <spoon/support/String.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Window.h>

#include <spoon/interface/TextInput.h>

TextInput::TextInput( Rect frame, uint32 follows )
: View( frame, follows )
{
	m_text = strdup("");
}

TextInput::~TextInput()
{
	free( m_text );
}

const char *TextInput::Text()
{
	return (const char*)m_text;
}

void TextInput::SetText( const char *text )
{
	if ( m_text != NULL ) free( m_text );

	if ( text == NULL ) m_text = strdup( "" );
				   else m_text = strdup( text );
	
	Draw( Bounds() );
    Sync();
}


void TextInput::Draw( Rect area )
{
   FillRect( Bounds(), 0x888888 );
   DrawRect( Bounds(), 0x999999 );
   DrawString( 2,2, m_text, 0 );
}

void TextInput::KeyDown( uint32 code, uint32 ascii, uint32 modifiers )
{
  char word[2];
  word[1] = 0;
  word[0] = (char)(ascii & 0xFF);
 
  switch (ascii)
  {
 
    case '\b':
        if ( strlen( m_text ) <= 0 ) break;
		m_text[ strlen(m_text) - 1 ] = 0;
		break;

    case '\r':
    case '\n':
    	break;

    default:
		m_text = (char*)realloc( m_text, strlen(m_text) + 2 );
		strcat( m_text, word);
		break;
	
  }

  Draw( Bounds() );
  Sync();
}

void TextInput::KeyUp( uint32 code, uint32 ascii, uint32 modifiers )
{
}




