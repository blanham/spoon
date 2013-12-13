#ifndef _SPOON_SUPPORT_STRING_H
#define _SPOON_SUPPORT_STRING_H

#include <types.h>


/** The String class is an abstraction of the String idea, allowing
 * for some nice and easy string manipulations using plain, old
 * operators.
 *
 * \code
 *
 * 		String bob = "hello";
 * 			   bob += ", Mr Frank.";
 * 			   bob  = bob + " You are ";
 * 			   bob << 63 << " years old!";
 *
 * \endcode
 *
 * Or at least I think it does. It should! But I haven't used it
 * in a while.
 *
 */

class String 
{

	public:
		String(void);
		String(const char *string);
		String(const char *string, int32 maxLength);
		String(const String &string);

		~String();

		String &Append(const String &source);
		String &Append(const String &source, int32 charCount);
		String &Append(const char *source);
		String &Append(const char *source, int32 charCount);
		String &Append(char c, int32 charCount);
		String &Prepend(const String &source);
		String &Prepend(const String &source, int32 charCount);
		String &Prepend(const char *source);
		String &Prepend(const char *source, int32 charCount);
		String &Prepend(char c, int32 charCount);
		String &Insert(const String &source, int32 insertAt);
		String &Insert(const String &source, int32 charCount, int32 insertAt);
		String &Insert(const String &source, int32 sourceOffset, int32 charCount, int32 insertAt);
		String &Insert(const char *source, int32 insertAt);
		String &Insert(const char *source, int32 charCount, int32 insertAt);
		String &Insert(const char *source, int32 sourceOffset, int32 charCount, int32 insertAt);
		String &Insert(char c, int32 charCount, int32 insertAt);

		String &CharacterEscape(const char *original, 
				         const char *setOfCharsToEscape,  
				         char escapeWithChar);
		String &CharacterEscape(const char *setOfCharsToEscape, char escapeWithChar);
		String &CharacterDeescape(const char *original, char escapeWithChar);
		String &CharacterDeescape(char escapeWithChar);

		int Compare(const String &string);
		int Compare(const String &string, int32 range);
		int Compare(const char *string);
		int Compare(const char *string, int32 range);
		int Compare(const String &astring, const String &bstring);
		int Compare(const String *astring, const String *bstring) ;
		int ICompare(const String &string);
		int ICompare(const String &string, int32 range);
		int ICompare(const char *string);
		int ICompare(const char *string, int32 range);
		int ICompare(const String &astring, const String &bstring) ;
		int ICompare(const String *astring, const String *bstring) ;
	
		String &CopyInto(String &destination, int32 sourceOffset, 
				      int32 charCount);
		void CopyInto(char *destination, int32 sourceOffset, int32 charCount);
		String &MoveInto(String &destination, int32 sourceOffset, int32 charCount);
		void MoveInto(char *destination, int32 sourceOffset, int32 charCount);
	
		int32 CountChars(void);
		int32 Length(void);
	
		int32 FindFirst(const String &string);
		int32 FindFirst(const String &string, int32 offset);
		int32 FindFirst(const char *string);
		int32 FindFirst(const char *string, int32 offset);
		int32 FindFirst(char c);
		int32 FindFirst(char c, int32 offset);
		int32 FindLast(const String &string);
		int32 FindLast(const String &string, int32 offset);
		int32 FindLast(const char *string);
		int32 FindLast(const char *string, int32 offset);
		int32 FindLast(char c);
		int32 FindLast(char c, int32 offset);
		int32 IFindFirst(const String &string);
		int32 IFindFirst(const String &string, int32 offset);
		int32 IFindFirst(const char *string);
		int32 IFindFirst(const char *string, int32 offset);
		int32 IFindLast(const String &string);
		int32 IFindLast(const String &string, int32 offset);
		int32 IFindLast(const char *string);
		int32 IFindLast(const char *string, int32 offset);

		char *Lockuffer(int32 maxLength);
		String &Unlockuffer(int32 length = -1);

		String &Remove(int32 startingAt, int32 charCount);
		String &RemoveFirst(const String &string);
		String &RemoveFirst(const char *string);
		String &RemoveLast(const String &string);
		String &RemoveLast(const char *string);
		String &RemoveAll(const String &string);
		String &RemoveAll(const char *string);
		String &RemoveSet(const char *charSet);
		String &RemoveSetTo(const char *charSet);
		String &RemoveSetTo(char charSet);
	
		String &Replace(const char *old, const char *nuwe, int32 count, int32 offset = 0);
		String &Replace(char old, char nuwe, int32 count, int32 offset = 0);
		String &ReplaceFirst(const char *old, const char *nuwe);
		String &ReplaceFirst(char old, char nuwe);
		String &ReplaceLast(const char *old, const char *nuwe);
		String &ReplaceLast(char old, char nuwe);
		String &ReplaceAll(const char *old, const char *nuwe, int32 offset = 0);
		String &ReplaceAll(char old, char nuwe, int32 offset = 0);
		String &ReplaceSet(const char *oldSet, const char *nuwe);
		String &ReplaceSet(const char *oldSet, char nuwe);
		String &ReplaceSetTo(const char *oldSet, const char *nuwe);
		String &ReplaceSetTo(const char *oldSet, char nuwe);
		String &IReplace(const char *old, const char *nuwe, int32 count, int32 offset = 0);
		String &IReplace(char old, char nuwe, int32 count, int32 offset = 0);
		String &IReplaceFirst(const char *old, const char *nuwe);
		String &IReplaceFirst(char old, char nuwe);
		String &IReplaceLast(const char *old, const char *nuwe);
		String &IReplaceLast(char old, char nuwe);
		String &IReplaceAll(const char *old, const char *nuwe, int32 offset = 0);
		String &IReplaceAll(char old, char nuwe, int32 offset = 0);

		String &SetTo(const char *source);
		String &SetTo(const char *source, int32 charCount);
		String &SetTo(const String &source);
		String &SetTo(const String &source, int32 charCount);
		String &SetTo(char c, int32 charCount);
		String &Adopt(String &source);
		String &Adopt(String &source, int32 charCount);

		const char* GetString();
		char ByteAt(int32 index);
	
		String &ToLower(void);
		String &ToUpper(void);
		String &Capitalize(void);
		String &CapitalizeEachWord(void);

		String &Truncate(int32 charCount, bool lazy = true);
	

		String& operator=(const String &string);
		String& operator=(const char *string);
		String& operator=(const char character);
		
		String& operator+=(const String &string);
		String& operator+=(const char *string);
		String& operator+=(const char character);
	
		String &operator<<(const char *string);
		String &operator<<(const String &string);
		String &operator<<(char c);
		String &operator<<(uint32 val);
		String &operator<<(int32 val);
		String &operator<<(uint64 val);
		String &operator<<(int64 val);
		String &operator<<(float val);
	
		char operator[](int32 index);
//		char &operator[](int32 index);
	
		bool operator==(const String &string);
		bool operator==(const char *string);
//		bool operator==(const char *string, const String &string);
		bool operator!=(const String &string);
		bool operator!=(const char *string);
//		bool operator!=(const char *string, const String &string);
		bool operator<(const String &string);
		bool operator<(const char *string);
//		bool operator<(const char *string, const String &string);
		bool operator<=(const String &string);
		bool operator<=(const char *string);
//		bool operator<=(const char *string, const String &string);
		bool operator>(const String &string);
		bool operator>(const char *string);
//		bool operator>(const char *string, const String &string);
		bool operator>=(const String &string);
		bool operator>=(const char *string);
//		bool operator>=(const char *string, const String &string);

	private:
		char *buffer;
		int length;
		int max;
		
};


#endif


