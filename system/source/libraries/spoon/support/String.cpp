#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <spoon/support/String.h>


String::String()
: buffer(NULL)
{
	buffer = strdup("");
	length = 0;
	max = -1;
}


String::String(const char *string)
: buffer(NULL)
{
	SetTo( string );
}


String::String(const char *string, int32 maxLength)
: buffer(NULL)
{
	max = maxLength;
	SetTo( string );
}


String::String(const String &string)
: buffer(NULL)
{
	SetTo( string );
}



String::~String()
{
   if ( this->buffer != NULL )	free( this->buffer );
}



String &String::Append(const String &source)
{
	int32 new_length;
	char *news;

	if ( source.length <= 0 ) return *this;

	new_length = length + source.length + 1;
	news = (char*)malloc( new_length );

	strcpy( news, buffer );
	strcat( news, source.buffer );

	free( buffer );
	buffer = news;
	length = new_length - 1;

	return *this;
}


String &String::Append(const String &source, int32 charCount)
{
	int32 new_length;
	char *news;
	int32 nl;

	nl = ((String)source).Length();
	if ( charCount < nl ) nl = charCount;

	new_length = length + nl + 1;
	news = (char*)malloc( new_length );

	strcpy( news, buffer );
	strncat( news, source.buffer, nl );
	
	free( buffer );
	buffer = news;
	length = strlen( buffer );

	return *this;
}


String &String::Append(const char *source)
{
	int32 new_length;
	char *news;

	new_length = length + strlen( source );
	news = (char*)malloc( new_length + 1 );

	strcpy( news, buffer );
	strcat( news, source );

	free( buffer );
	buffer = news;
	length = strlen( buffer );

	return *this;
}


String &String::Append(const char *source, int32 charCount)
{
	int32 new_length;
	char *news;
	int nl;

	nl = strlen( source );
	if ( charCount < nl ) nl = charCount;

	new_length = length + nl;
	news = (char*)malloc( new_length + 1 );

	strcpy( news, buffer );
	strncat( news, source, nl );

	free( buffer );
	buffer = news;
	length = strlen( buffer );

	return *this;
}


String &String::Append(char c, int32 charCount)
{
	return *this;
}


String &String::Prepend(const String &source)
{
	return *this;
}


String &String::Prepend(const String &source, int32 charCount)
{
	return *this;
}


String &String::Prepend(const char *source)
{
	return *this;
}


String &String::Prepend(const char *source, int32 charCount)
{
	return *this;
}


String &String::Prepend(char c, int32 charCount)
{
	return *this;
}


String &String::Insert(const String &source, int32 insertAt)
{
	return *this;
}


String &String::Insert(const String &source, int32 charCount, int32 insertAt)
{
	return *this;
}


String &String::Insert(const String &source, int32 sourceOffset, int32 charCount, int32 insertAt)
{
	return *this;
}


String &String::Insert(const char *source, int32 insertAt)
{
    if ( insertAt >= Length() ) return Append(source);

    int new_len = Length() + strlen(source) + 1;
    int src_len = strlen(source);
    char *new_buf = (char*)malloc( new_len );

	    for ( int i = 0; i < insertAt; i++ )
	      new_buf[i] = buffer[i];
     
	    for ( int j = 0; j < src_len; j++ )
	      new_buf[j + insertAt] = source[j];
     
	    for ( int i = 0; i < insertAt; i++ )
	      new_buf[i + insertAt + src_len] = buffer[i + insertAt];
   
      new_buf[ new_len - 1 ]  = 0;
   
    delete buffer;
    buffer = new_buf;
    
    return *this;
}


String &String::Insert(const char *source, int32 charCount, int32 insertAt)
{
	return *this;
}


String &String::Insert(const char *source, int32 sourceOffset, int32 charCount, int32 insertAt)
{
	return *this;
}


String &String::Insert(char c, int32 charCount, int32 insertAt)
{
	return *this;
}



String &String::CharacterEscape(const char *original, 
			const char *setOfCharsToEscape,  
			char escapeWithChar)
{
	return *this;
}


String &String::CharacterEscape(const char *setOfCharsToEscape, char escapeWithChar)
{
	return *this;
}


String &String::CharacterDeescape(const char *original, char escapeWithChar)
{
	return *this;
}


String &String::CharacterDeescape(char escapeWithChar)
{
	return *this;
}



int String::Compare(const String &string)
{
	return 0;
}


int String::Compare(const String &string, int32 range)
{
	return 0;
}


int String::Compare(const char *string)
{
	return 0;
}


int String::Compare(const char *string, int32 range)
{
	return 0;
}


int String::Compare(const String &astring, const String &bstring) 
{
	return 0;
}


int String::Compare(const String *astring, const String *bstring) 
{
	return 0;
}


int String::ICompare(const String &string)
{
	return 0;
}


int String::ICompare(const String &string, int32 range)
{
	return 0;
}


int String::ICompare(const char *string)
{
	return 0;
}


int String::ICompare(const char *string, int32 range)
{
	return 0;
}


int String::ICompare(const String &astring, const String &bstring) 
{
	return 0;
}


int String::ICompare(const String *astring, const String *bstring) 
{
	return 0;
}



String &String::CopyInto(String &destination, int32 sourceOffset, 
		int32 charCount)
{
	return *this;
}


void String::CopyInto(char *destination, int32 sourceOffset, int32 charCount)
{
}


String &String::MoveInto(String &destination, int32 sourceOffset, int32 charCount)
{
	return *this;
}


void String::MoveInto(char *destination, int32 sourceOffset, int32 charCount)
{
}



int32 String::CountChars()
{
	return strlen( buffer );
}


int32 String::Length()
{
	return strlen( buffer );
}



int32 String::FindFirst(const String &string)
{
	return 0;
}


int32 String::FindFirst(const String &string, int32 offset)
{
	return 0;
}


int32 String::FindFirst(const char *string)
{
	return 0;
}


int32 String::FindFirst(const char *string, int32 offset)
{
	return 0;
}


int32 String::FindFirst(char c)
{
	return 0;
}


int32 String::FindFirst(char c, int32 offset)
{
	return 0;
}


int32 String::FindLast(const String &string)
{
	return 0;
}


int32 String::FindLast(const String &string, int32 offset)
{
	return 0;
}


int32 String::FindLast(const char *string)
{
	return 0;
}


int32 String::FindLast(const char *string, int32 offset)
{
	return 0;
}


int32 String::FindLast(char c)
{
	return 0;
}


int32 String::FindLast(char c, int32 offset)
{
	return 0;
}


int32 String::IFindFirst(const String &string)
{
	return 0;
}


int32 String::IFindFirst(const String &string, int32 offset)
{
	return 0;
}


int32 String::IFindFirst(const char *string)
{
	return 0;
}


int32 String::IFindFirst(const char *string, int32 offset)
{
	return 0;
}


int32 String::IFindLast(const String &string)
{
	return 0;
}


int32 String::IFindLast(const String &string, int32 offset)
{
	return 0;
}


int32 String::IFindLast(const char *string)
{
	return 0;
}


int32 String::IFindLast(const char *string, int32 offset)
{
	return 0;
}



char *String::Lockuffer(int32 maxLength)
{
	return NULL;
}


String &String::Unlockuffer(int32 length)
{
	return *this;
}



String &String::Remove(int32 startingAt, int32 charCount)
{
	return *this;
}


String &String::RemoveFirst(const String &string)
{
	return *this;
}


String &String::RemoveFirst(const char *string)
{
	return *this;
}


String &String::RemoveLast(const String &string)
{
	return *this;
}


String &String::RemoveLast(const char *string)
{
	return *this;
}


String &String::RemoveAll(const String &string)
{
	return *this;
}


String &String::RemoveAll(const char *string)
{
	return *this;
}


String &String::RemoveSetTo(const char *charSet)
{
	return *this;
}



String &String::Replace(const char *old, const char *nuwe, int32 count, int32 offset)
{
	return *this;
}


String &String::Replace(char old, char nuwe, int32 count, int32 offset)
{
	return *this;
}


String &String::ReplaceFirst(const char *old, const char *nuwe)
{
	return *this;
}


String &String::ReplaceFirst(char old, char nuwe)
{
	return *this;
}


String &String::ReplaceLast(const char *old, const char *nuwe)
{
	return *this;
}


String &String::ReplaceLast(char old, char nuwe)
{
	return *this;
}


String &String::ReplaceAll(const char *old, const char *nuwe, int32 offset)
{
	return *this;
}


String &String::ReplaceAll(char old, char nuwe, int32 offset)
{
	return *this;
}


String &String::ReplaceSetTo(const char *oldSet, const char *nuwe)
{
	return *this;
}


String &String::ReplaceSetTo(const char *oldSet, char nuwe)
{
	return *this;
}


String &String::IReplace(const char *old, const char *nuwe, int32 count, int32 offset)
{
	return *this;
}


String &String::IReplace(char old, char nuwe, int32 count, int32 offset)
{
	return *this;
}


String &String::IReplaceFirst(const char *old, const char *nuwe)
{
	return *this;
}


String &String::IReplaceFirst(char old, char nuwe)
{
	return *this;
}


String &String::IReplaceLast(const char *old, const char *nuwe)
{
	return *this;
}


String &String::IReplaceLast(char old, char nuwe)
{
	return *this;
}


String &String::IReplaceAll(const char *old, const char *nuwe, int32 offset)
{
	return *this;
}


String &String::IReplaceAll(char old, char nuwe, int32 offset)
{
	return *this;
}



String &String::SetTo(const char *source)
{
	buffer = strdup( source );
	length = strlen( buffer );
	max = 0;
	return *this;
}


String &String::SetTo(const char *source, int32 charCount)
{
	max = charCount;
	buffer = strdup( source );
	length = strlen( buffer );
	return *this;
}


String &String::SetTo(const String &source)
{	
	max = source.max;
	buffer = strdup( source.buffer );
	length = source.length;
	return *this;
}


String &String::SetTo(const String &source, int32 charCount)
{
	max = charCount;
	buffer = strdup( source.buffer );
	length = source.length;
	return *this;
}


String &String::SetTo(char c, int32 charCount)
{
	char b[2];
	b[0] = c;
	b[1] = 0;
	SetTo( b, charCount );
	return *this;
}


String &String::Adopt(String &source)
{
	return *this;
}


String &String::Adopt(String &source, int32 charCount)
{
	return *this;
}



const char* String::GetString()
{
	return (const char*)buffer;
}


char String::ByteAt(int32 index)
{
	if( index >= Length() ) return 0;
	return buffer[index];
}



String &String::ToLower()
{
	int i;
	int len = strlen(buffer);
	

	for ( i = 0; i < len; i++)
	{
		if ( (buffer[i] <= 'Z') && (buffer[i] >= 'A')) buffer[i] = buffer[i] + 'a' - 'A';
	}

	return *this;
}


String &String::ToUpper()
{
	int i;
	int len = strlen(buffer);

	for ( i = 0; i < len; i++)
	{
		if ( (buffer[i] <= 'z') && (buffer[i] >= 'a')) buffer[i] = buffer[i] + 'A' - 'a';
	}

	return *this;
}


String &String::Capitalize()
{
	return *this;
}


String &String::CapitalizeEachWord()
{
	return *this;
}



String &String::Truncate(int32 charCount, bool lazy)
{
	if ( charCount >= Length() ) return *this;

	buffer[charCount] = 0;
	return *this;
}




String& String::operator=(const String &string)
{
	SetTo( string );
	return *this;
}


String& String::operator=(const char *string)
{
	SetTo( string );
	return *this;
}


String& String::operator=(const char character)
{
	char bob[2];
	bob[0] = character;
	bob[1] = 0;
	SetTo( bob );
	return *this;
}



String& String::operator+=(const String &string)
{
	Append( string );
	return *this;
}


String& String::operator+=(const char *string)
{
	Append( string );
	return *this;
}


String& String::operator+=(const char character)
{
	char bob[2];
	bob[0] = character;
	bob[1] = 0;
	Append( bob );
	return *this;
}



String &String::operator<<(const char *string)
{
	Append( string );
	return *this;
}


String &String::operator<<(const String &string)
{
	Append( string );
	return *this;
}


String &String::operator<<(char c)
{
	char bob[2];
	bob[0] = c;
	bob[1] = 0;
	Append( bob );
	return *this;
}


String &String::operator<<(uint32 val)
{
   char number[32];
   sprintf(number, "%u", val );
   Append( number );
   return *this;
}


String &String::operator<<(int32 val)
{
   char number[32];
   sprintf(number, "%i", val );
   Append( number );
   return *this;
}


String &String::operator<<(uint64 val)
{
   char number[32];
   sprintf(number, "%u", val );
   Append( number );
   return *this;
}


String &String::operator<<(int64 val)
{
   char number[32];
   sprintf(number, "%i", val );
   Append( number );
   return *this;
}


String &String::operator<<(float val)
{
   char number[32];
   sprintf(number, "%f", val );
   Append( number );
   return *this;
}



char String::operator[](int32 index)
{
	return ByteAt( index );
}


//char &String::operator[](int32 index)
//{
//	char *heck;
//	heck = &(buffer[index]);
//	return *heck;
//}



bool String::operator==(const String &string)
{
   if ( string.length != length ) return false;
   if ( string.length == 0 ) return true;
   if ( strcmp( buffer, string.buffer ) == 0 ) return true;
   return false;
}


bool String::operator==(const char *string)
{
   if ( (string == NULL) && (buffer == NULL) ) return true;
   if ( (string == NULL) || (buffer == NULL) ) return false;
   if ( (int)strlen( string ) != length ) return false;
   if ( strcmp( buffer, string ) == 0 ) return true;
   return false;
}


//bool String::operator==(const char *string, String &string)
//{
//}


bool String::operator!=(const String &string)
{
	return ( ! (*this == string) );
}


bool String::operator!=(const char *string)
{
	return ( ! (*this == string) );
}


//bool String::operator!=(const char *string, String &string)
//{
//}


bool String::operator<(const String &string)
{
   int leftl = length;
   int rightl = string.length;

   int i = 0;


    while (  (i < leftl) && (i < rightl) )
    {
       if ( buffer[i] < string.buffer[i] ) return true;
       if ( buffer[i] > string.buffer[i] ) return false;
       i++;
    }

   
    if ( leftl < rightl ) return true;
    return false;
}


bool String::operator<(const char *string)
{
   int leftl = length;
   int rightl = strlen( string );

   int i = 0;


    while (  (i < leftl) && (i < rightl) )
    {
       if ( buffer[i] < string[i] ) return true;
       if ( buffer[i] > string[i] ) return false;
       i++;
    }

   
    if ( leftl < rightl ) return true;
    return false;
}


//bool String::operator<(const char *string, String &string)
//{
//}


bool String::operator<=(const String &string)
{
   if ( length != string.length )
   	return (*this < string);
 
   return (*this == string);
}


bool String::operator<=(const char *string)
{
   if ( length != (int)strlen( string ) )
   	return (*this < string);
 
   return (*this == string);
}


//bool String::operator<=(const char *string, String &string)
//{
//}


bool String::operator>(const String &string)
{
   int leftl = length;
   int rightl = string.length;

   int i = 0;


    while (  (i < leftl) && (i < rightl) )
    {
       if ( buffer[i] < string.buffer[i] ) return false;
       if ( buffer[i] > string.buffer[i] ) return true;
       i++;
    }

   
    if ( leftl > rightl ) return true;
    return false;
}


bool String::operator>(const char *string)
{
   int leftl = length;
   int rightl = strlen( string );

   int i = 0;


    while (  (i < leftl) && (i < rightl) )
    {
       if ( buffer[i] < string[i] ) return false;
       if ( buffer[i] > string[i] ) return true;
       i++;
    }

   
    if ( leftl > rightl ) return true;
    return false;

}


//bool String::operator>(const char *string, String &string)
//{
//}


bool String::operator>=(const String &string)
{
   return (!(*this < string) );
}


bool String::operator>=(const char *string)
{
   return (!(*this < string) );
}


//bool String::operator>=(const char *string, String &string)
//{
//}






